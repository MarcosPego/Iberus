#include "Enginepch.h"
#include "SDFRenderSystem.h"
#include "Components.h"
#include "Scene.h"
#include "Engine.h"
#include "Material.h"
#include "RenderBatch.h"
#include "RenderCmd.h"
#include "OpenGLRaymarchingPass.h"
#include "RenderSettings.h"

#include <algorithm>
#include <cstring>

using namespace Math;

namespace Iberus {

	namespace {
		void ExtractFrustumPlanes(const Mat4& viewProj, Vec4 planes[6]) {
			const float* d = viewProj.data;
			planes[0] = Vec4(d[3] + d[0], d[7] + d[4], d[11] + d[8],  d[15] + d[12]);  // left
			planes[1] = Vec4(d[3] - d[0], d[7] - d[4], d[11] - d[8],  d[15] - d[12]);  // right
			planes[2] = Vec4(d[3] + d[1], d[7] + d[5], d[11] + d[9],  d[15] + d[13]);  // bottom
			planes[3] = Vec4(d[3] - d[1], d[7] - d[5], d[11] - d[9],  d[15] - d[13]);  // top
			planes[4] = Vec4(d[3] + d[2], d[7] + d[6], d[11] + d[10], d[15] + d[14]);  // near
			planes[5] = Vec4(d[3] - d[2], d[7] - d[6], d[11] - d[10], d[15] - d[14]);  // far
			for (int i = 0; i < 6; ++i) {
				float len = std::sqrt(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
				if (len > 1e-6f) {
					planes[i].x /= len;
					planes[i].y /= len;
					planes[i].z /= len;
					planes[i].w /= len;
				}
			}
		}

		bool SphereOutsideFrustum(const Vec3& center, float radius, const Vec4 planes[6]) {
			for (int i = 0; i < 6; ++i) {
				float sd = center.x * planes[i].x + center.y * planes[i].y + center.z * planes[i].z + planes[i].w;
				if (sd < -radius) {
					return true;
				}
			}
			return false;
		}
	}

	void SDFRenderSystem::Execute(World& world, Scene& scene, RenderBatch& renderBatch) {
		auto* sdfStorage = world.GetStorage<SDFComponent>();
		auto* activeStorage = world.GetStorage<ActiveComponent>();
		if (!sdfStorage) {
			return;
		}

		Vec4 frustumPlanes[6];
		const CameraRenderCmd* cameraRenderCmd = renderBatch.GetCameraRenderCmd();
		if (cameraRenderCmd) {
			Mat4 viewProj = cameraRenderCmd->projectionMatrix * cameraRenderCmd->viewMatrix;
			ExtractFrustumPlanes(viewProj, frustumPlanes);
		}

		auto sdfBufferCmd = std::make_unique<SDFBufferRenderCmd>();
		sdfBufferCmd->uboData.resize(SDFSSBO::BLOCK_SIZE, 0);

		const float radiusScale = Engine::Instance()->GetPostProcessSettings().sdfRadiusScale;
		int sdfSlot = 0;
		int totalPartCount = 0;
		auto& resourceManager = Engine::Instance()->GetResourceManager();
		auto* provider = &Engine::Instance()->GetEngineProvider();

		for (auto [entityId, sdf] : *sdfStorage) {
			if (sdfSlot >= static_cast<int>(SDFSSBO::MAX_CREATURES)) {
				break;
			}
			if (totalPartCount >= static_cast<int>(SDFSSBO::POOL_BUDGET)) {
				break;
			}
			if (!world.IsAlive(entityId)) {
				continue;
			}
			if (activeStorage) {
				auto* active = activeStorage->Get(entityId);
				if (active && !active->Active) {
					continue;
				}
			}
			if (sdf.Parts.empty()) {
				continue;
			}

			auto* localToWorld = world.GetComponent<LocalToWorldComponent>(entityId);
			if (!localToWorld) {
				continue;
			}

			Material* entityMaterial = nullptr;
			auto* meshRenderer = world.GetComponent<MeshRendererComponent>(entityId);
			if (meshRenderer && !meshRenderer->MaterialId.empty()) {
				entityMaterial = resourceManager.GetOrCreateResource<Material>(meshRenderer->MaterialId, provider);
			}

			const int partLimit = std::min(static_cast<int>(sdf.Parts.size()),
				static_cast<int>(SDFSSBO::POOL_BUDGET - totalPartCount));
			const int partCount = partLimit > 0 ? partLimit : 0;
			if (partCount <= 0) {
				continue;
			}

			// Compute world-space bounding sphere for culling
			Vec3 boundMin(1e30f, 1e30f, 1e30f);
			Vec3 boundMax(-1e30f, -1e30f, -1e30f);
			float maxPartRadius = 0.0f;
			const Mat4& m = localToWorld->Matrix;
			for (int idx = 0; idx < partCount; ++idx) {
				const auto& p = sdf.Parts[idx];
				Vec3 worldCenter = Vec3(m * Vec4(p.Transform.Position));
				boundMin.x = std::min(boundMin.x, worldCenter.x);
				boundMin.y = std::min(boundMin.y, worldCenter.y);
				boundMin.z = std::min(boundMin.z, worldCenter.z);
				boundMax.x = std::max(boundMax.x, worldCenter.x);
				boundMax.y = std::max(boundMax.y, worldCenter.y);
				boundMax.z = std::max(boundMax.z, worldCenter.z);
				if (p.Type == 3) {
					Vec3 worldEnd = Vec3(m * Vec4(p.Endpoint));
					boundMin.x = std::min(boundMin.x, worldEnd.x);
					boundMin.y = std::min(boundMin.y, worldEnd.y);
					boundMin.z = std::min(boundMin.z, worldEnd.z);
					boundMax.x = std::max(boundMax.x, worldEnd.x);
					boundMax.y = std::max(boundMax.y, worldEnd.y);
					boundMax.z = std::max(boundMax.z, worldEnd.z);
				}
				maxPartRadius = std::max(maxPartRadius, p.Radius);
			}
			// Extract uniform scale from LocalToWorld (column-major upper-left 3x3)
			const float* d = localToWorld->Matrix.data;
			const float scaleX = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
			const float scaleY = std::sqrt(d[4] * d[4] + d[5] * d[5] + d[6] * d[6]);
			const float scaleZ = std::sqrt(d[8] * d[8] + d[9] * d[9] + d[10] * d[10]);
			const float uniformScale = (scaleX + scaleY + scaleZ) / 3.0f;

			Vec3 boundCenter = (boundMin + boundMax) * 0.5f;
			float boundRadius = (boundMax - boundMin).length() * 0.5f + maxPartRadius * uniformScale * radiusScale;

			// Frustum culling
			const float cullMargin = 1.5f;
			if (cameraRenderCmd && SphereOutsideFrustum(boundCenter, boundRadius * cullMargin, frustumPlanes)) {
				continue;
			}

			// Write creature header (pool layout)
			const size_t headerBase = SDFSSBO::GLOBAL_HEADER_SIZE + sdfSlot * SDFSSBO::CREATURE_HEADER_STRIDE;
			*reinterpret_cast<int*>(sdfBufferCmd->uboData.data() + headerBase + SDFSSBO::CREATURE_HEADER_PART_OFFSET) = totalPartCount;
			*reinterpret_cast<int*>(sdfBufferCmd->uboData.data() + headerBase + SDFSSBO::CREATURE_HEADER_PART_COUNT) = partCount;
			const Vec4 boundCenter4(boundCenter.x, boundCenter.y, boundCenter.z, 1.0f);
			std::memcpy(sdfBufferCmd->uboData.data() + headerBase + SDFSSBO::CREATURE_HEADER_BOUND_CENTER, &boundCenter4, 16);
			*reinterpret_cast<float*>(sdfBufferCmd->uboData.data() + headerBase + SDFSSBO::CREATURE_HEADER_BOUND_RADIUS) = boundRadius;

			for (int j = 0; j < partCount; ++j) {
				const auto& part = sdf.Parts[j];
				Vec4 localCenter(part.Transform.Position);
				Vec4 worldCenter4 = localToWorld->Matrix * localCenter;
				Vec3 center(worldCenter4);

				Vec3 endpoint = center;
				if (part.Type == 3) {
					Vec4 localEndpoint(part.Endpoint);
					Vec4 worldEndpoint4 = localToWorld->Matrix * localEndpoint;
					endpoint = Vec3(worldEndpoint4);
				}

				Vec4 color = Vec4(1, 1, 1, 1);
				if (!part.MaterialId.empty()) {
					Material* partMat = resourceManager.GetOrCreateResource<Material>(part.MaterialId, provider);
					if (partMat) {
						color = partMat->albedoColor;
					}
				} else if (entityMaterial) {
					color = entityMaterial->albedoColor;
				}

				const size_t partBase = SDFSSBO::PARTS_OFFSET + (totalPartCount + j) * SDFSSBO::SDF_PART_STRIDE;
				const Vec4 center4(center.x, center.y, center.z, 1.0f);
				const Vec4 endpoint4(endpoint.x, endpoint.y, endpoint.z, 1.0f);
				const float worldRadius = part.Radius * uniformScale * radiusScale;
				std::memcpy(sdfBufferCmd->uboData.data() + partBase + SDFSSBO::PART_CENTER, &center4, 16);
				std::memcpy(sdfBufferCmd->uboData.data() + partBase + SDFSSBO::PART_COLOR, &color, 16);
				*reinterpret_cast<float*>(sdfBufferCmd->uboData.data() + partBase + SDFSSBO::PART_RADIUS) = worldRadius;
				*reinterpret_cast<int*>(sdfBufferCmd->uboData.data() + partBase + SDFSSBO::PART_TYPE) = part.Type;
				*reinterpret_cast<uint32_t*>(sdfBufferCmd->uboData.data() + partBase + SDFSSBO::PART_BLEND_GROUP_MASK) = part.BlendGroupMask != 0u ? part.BlendGroupMask : 0x01u;
				std::memcpy(sdfBufferCmd->uboData.data() + partBase + SDFSSBO::PART_ENDPOINT, &endpoint4, 16);
			}

			totalPartCount += partCount;
			sdfSlot++;
		}

		// Write global header
		*reinterpret_cast<int*>(sdfBufferCmd->uboData.data() + SDFSSBO::GLOBAL_CREATURE_COUNT) = sdfSlot;
		*reinterpret_cast<int*>(sdfBufferCmd->uboData.data() + SDFSSBO::GLOBAL_TOTAL_PART_COUNT) = totalPartCount;

		// Shrink to exact needed bytes for upload. Parts are at PARTS_OFFSET (fixed layout).
		const size_t requiredBytes = SDFSSBO::PARTS_OFFSET
			+ static_cast<size_t>(totalPartCount) * SDFSSBO::SDF_PART_STRIDE;
		sdfBufferCmd->uboData.resize(requiredBytes);

		renderBatch.PushRenderCmdToQueue(std::move(sdfBufferCmd), CMDQueue::SDF);
	}

}
