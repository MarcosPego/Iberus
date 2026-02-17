#include "Enginepch.h"
#include "SDFRenderSystem.h"
#include "Components.h"
#include "Scene.h"
#include "Material.h"
#include "RenderBatch.h"
#include "RenderCmd.h"

using namespace Math;

namespace Iberus {

	// Must match sdfMeshBufferSize in baseRaymarchingShader.frag
	constexpr int SDF_MESH_BUFFER_SIZE = 3;

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
			auto* meshRenderer = world.GetComponent<MeshRendererComponent>(entityId);
			if (meshRenderer && !meshRenderer->MaterialId.empty()) {
				entityMaterial = scene.GetOrCreateMaterial<Material>(meshRenderer->MaterialId);
			}

			const std::string sdfMesh = std::format("sdfMeshes[{0}]", sdfSlot);
			renderBatch.PushRenderCmdToQueue(
				std::make_unique<UniformRenderCmd<int>>(sdfMesh + ".size", static_cast<int>(sdf.Parts.size()), UniformType::INT),
				CMDQueue::SDF);

			int count = 0;
			for (const auto& part : sdf.Parts) {
				Vec4 localCenter(part.Transform.Position);
				Vec4 worldCenter4 = localToWorld->Matrix * localCenter;
				Vec3 center(worldCenter4);

				std::string sdfPart = std::format("{0}.sdfParts[{1}]", sdfMesh, count);
				renderBatch.PushRenderCmdToQueue(
					std::make_unique<UniformRenderCmd<int>>(sdfPart + ".type", part.Type, UniformType::INT), CMDQueue::SDF);
				renderBatch.PushRenderCmdToQueue(
					std::make_unique<UniformRenderCmd<float>>(sdfPart + ".radius", part.Radius, UniformType::FLOAT), CMDQueue::SDF);
				renderBatch.PushRenderCmdToQueue(
					std::make_unique<UniformRenderCmd<Vec3>>(sdfPart + ".center", center, UniformType::VEC3), CMDQueue::SDF);

				Vec4 color = Vec4(1, 1, 1, 1);
				if (!part.MaterialId.empty()) {
					Material* partMat = scene.GetOrCreateMaterial<Material>(part.MaterialId);
					if (partMat) {
						color = partMat->albedoColor;
					}
				} else if (entityMaterial) {
					color = entityMaterial->albedoColor;
				}
				renderBatch.PushRenderCmdToQueue(
					std::make_unique<UniformRenderCmd<Vec4>>(sdfPart + ".color", color, UniformType::VEC4), CMDQueue::SDF);
				count++;
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
