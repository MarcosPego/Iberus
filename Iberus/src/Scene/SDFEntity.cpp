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

		/// TODO(MPP) A string formater here would be nice
		const std::string sdfMesh = std::format("sdfMeshes[{0}]", sdfSlot);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfMesh + ".type", type, UniformType::INT), CMDQueue::SDF);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfMesh + ".radius", radius, UniformType::FLOAT), CMDQueue::SDF);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfMesh + ".center", GetPosition(), UniformType::VEC3), CMDQueue::SDF);

		auto* material = GetMaterial();
		if (material) {
			renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfMesh + ".color", material->albedoColor, UniformType::VEC4), CMDQueue::SDF);
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
}