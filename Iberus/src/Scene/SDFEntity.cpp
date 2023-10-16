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
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfMesh + ".size", 3, UniformType::INT), CMDQueue::SDF);

		std::string sdfPart = std::format("{0}.sdfParts[{1}]", sdfMesh, 0);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".type", type, UniformType::INT), CMDQueue::SDF);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".radius", 0.5f, UniformType::FLOAT), CMDQueue::SDF);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".center", GetPosition() + Vec3(-2, 0, 0), UniformType::VEC3), CMDQueue::SDF);

		auto* material = GetMaterial();
		if (material) {
			renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".color", material->albedoColor, UniformType::VEC4), CMDQueue::SDF);
		}

		sdfPart = std::format("{0}.sdfParts[{1}]", sdfMesh, 1);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".type", type, UniformType::INT), CMDQueue::SDF);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".radius", 1.5f, UniformType::FLOAT), CMDQueue::SDF);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".center", GetPosition() + Vec3(2,0,0), UniformType::VEC3), CMDQueue::SDF);

		if (material) {
			renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".color", Vec4( 1.0f, 0.0f, 0.0f, 1.0f ), UniformType::VEC4), CMDQueue::SDF);
		}

		sdfPart = std::format("{0}.sdfParts[{1}]", sdfMesh, 2);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".type", 2, UniformType::INT), CMDQueue::SDF);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".radius", 2.0f, UniformType::FLOAT), CMDQueue::SDF);
		renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".center", GetPosition() + Vec3(0, -3, 0), UniformType::VEC3), CMDQueue::SDF);

		if (material) {
			renderBatch.PushRenderCmdToQueue(new UniformRenderCmd(sdfPart + ".color", Vec4(0.0f, 1.0f, 0.5f, 1.0f), UniformType::VEC4), CMDQueue::SDF);
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