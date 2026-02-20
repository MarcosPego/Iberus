#include "Enginepch.h"
#include "SDFRenderSystem.h"
#include "Components.h"
#include "Scene.h"
#include "Engine.h"
#include "Material.h"
#include "RenderBatch.h"
#include "RenderCmd.h"

using namespace Math;

namespace Iberus {

	// Must match sdfMeshBufferSize in baseRaymarchingShader.frag
	constexpr int SDF_MESH_BUFFER_SIZE = 4;

	void SDFRenderSystem::Execute(World& world, Scene& scene, RenderBatch& renderBatch) {
		auto* sdfStorage = world.GetStorage<SDFComponent>();
		auto* activeStorage = world.GetStorage<ActiveComponent>();
		if (!sdfStorage) {
			return;
		}

		int sdfSlot = 0;
		for (auto [entityId, sdf] : *sdfStorage) {
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
			auto& resourceManager = Engine::Instance()->GetResourceManager();
			auto* provider = &Engine::Instance()->GetEngineProvider();
			auto* meshRenderer = world.GetComponent<MeshRendererComponent>(entityId);
			if (meshRenderer && !meshRenderer->MaterialId.empty()) {
				entityMaterial = resourceManager.GetOrCreateResource<Material>(meshRenderer->MaterialId, provider);
			}

			constexpr int MAX_PARTS = 16; // Must match sdfPartBufferSize in shader
			const int partCount = static_cast<int>(std::min(sdf.Parts.size(), static_cast<size_t>(MAX_PARTS)));

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
			Vec3 boundCenter = (boundMin + boundMax) * 0.5f;
			float boundRadius = (boundMax - boundMin).length() * 0.5f + maxPartRadius;

			const std::string sdfMesh = std::format("sdfMeshes[{0}]", sdfSlot);
			renderBatch.PushRenderCmdToQueue(
				std::make_unique<UniformRenderCmd<int>>(sdfMesh + ".size", partCount, UniformType::INT),
				CMDQueue::SDF);
			renderBatch.PushRenderCmdToQueue(
				std::make_unique<UniformRenderCmd<Vec3>>(sdfMesh + ".boundCenter", boundCenter, UniformType::VEC3),
				CMDQueue::SDF);
			renderBatch.PushRenderCmdToQueue(
				std::make_unique<UniformRenderCmd<float>>(sdfMesh + ".boundRadius", boundRadius, UniformType::FLOAT),
				CMDQueue::SDF);

			for (int count = 0; count < partCount; ++count) {
				const auto& part = sdf.Parts[count];
				Vec4 localCenter(part.Transform.Position);
				Vec4 worldCenter4 = localToWorld->Matrix * localCenter;
				Vec3 center(worldCenter4);

				Vec3 endpoint = center;
				if (part.Type == 3) {
					Vec4 localEndpoint(part.Endpoint);
					Vec4 worldEndpoint4 = localToWorld->Matrix * localEndpoint;
					endpoint = Vec3(worldEndpoint4);
				}

				std::string sdfPart = std::format("{0}.sdfParts[{1}]", sdfMesh, count);
				renderBatch.PushRenderCmdToQueue(
					std::make_unique<UniformRenderCmd<int>>(sdfPart + ".type", part.Type, UniformType::INT), CMDQueue::SDF);
				renderBatch.PushRenderCmdToQueue(
					std::make_unique<UniformRenderCmd<float>>(sdfPart + ".radius", part.Radius, UniformType::FLOAT), CMDQueue::SDF);
				renderBatch.PushRenderCmdToQueue(
					std::make_unique<UniformRenderCmd<Vec3>>(sdfPart + ".center", center, UniformType::VEC3), CMDQueue::SDF);
				renderBatch.PushRenderCmdToQueue(
					std::make_unique<UniformRenderCmd<Vec3>>(sdfPart + ".endpoint", endpoint, UniformType::VEC3), CMDQueue::SDF);

				Vec4 color = Vec4(1, 1, 1, 1);
				if (!part.MaterialId.empty()) {
					Material* partMat = resourceManager.GetOrCreateResource<Material>(part.MaterialId, provider);
					if (partMat) {
						color = partMat->albedoColor;
					}
				} else if (entityMaterial) {
					color = entityMaterial->albedoColor;
				}
				renderBatch.PushRenderCmdToQueue(
					std::make_unique<UniformRenderCmd<Vec4>>(sdfPart + ".color", color, UniformType::VEC4), CMDQueue::SDF);
			}
			sdfSlot++;
		}

		// Clear unused slots so deleted entities don't leave "ghosts" (stale GPU uniform data).
		for (int slot = sdfSlot; slot < SDF_MESH_BUFFER_SIZE; ++slot) {
			const std::string sdfMesh = std::format("sdfMeshes[{0}]", slot);
			renderBatch.PushRenderCmdToQueue(
				std::make_unique<UniformRenderCmd<int>>(sdfMesh + ".size", 0, UniformType::INT),
				CMDQueue::SDF);
		}
	}

}
