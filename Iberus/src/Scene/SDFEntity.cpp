#include "Enginepch.h"
#include "SDFEntity.h"

#include "RenderBatch.h"
#include "RenderCmd.h"

#include "Material.h"

namespace Iberus {

	SDFEntity::SDFEntity(const std::string& ID, Scene* inScene) : Entity(ID, inScene) {
	}

	void SDFEntity::PushDrawSDF(RenderBatch& renderBatch, int sdfSlot) {
		if (!GetActive()) {
			return;
		}

		if (sdfParts.empty()) {
			return;
		}

		const std::string sdfMesh = std::format("sdfMeshes[{0}]", sdfSlot);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfMesh + ".size", (int)sdfParts.size(), UniformType::INT), CMDQueue::SDF);
		int count = 0;
		for (const auto& part : sdfParts) {
			auto _center = GetPosition() + part.transform.Position;

			std::string sdfPart = std::format("{0}.sdfParts[{1}]", sdfMesh, count);
			renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".type", part.type, UniformType::INT), CMDQueue::SDF);
			renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".radius", part.radius, UniformType::FLOAT), CMDQueue::SDF);
			renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".center", _center, UniformType::VEC3), CMDQueue::SDF);

			auto* material = GetMaterial();
			if (part.material) {
				renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".color", part.material->albedoColor, UniformType::VEC4), CMDQueue::SDF);
			}
			else if (material) {
				renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".color", material->albedoColor, UniformType::VEC4), CMDQueue::SDF);
			}
			count++;
		}
	}

	void SDFEntity::PushDraw(RenderBatch& renderBatch) {
		if (!GetActive()) {
			return;
		}

		for (const auto& child : GetChildMap()) {
			child.second->PushDraw(renderBatch);
		}
	}

	void SDFEntity::PushPart(Vec3 transform, int type, float radius, Material* material) {
		SDFPart part = {};
		part.SetPosition(transform);
		part.type = type;
		part.radius = radius;
		part.material = material;

		sdfParts.push_back(part);
	}
}