#include "Enginepch.h"
#include "MeshRenderSystem.h"
#include "Scene.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderBatch.h"
#include "RenderCmd.h"
#include "Engine.h"
#include "Components.h"

using namespace Math;

namespace Iberus {

	static void PushDrawRecursive(World& world, Scene& scene, RenderBatch& renderBatch, EntityId entityId) {
		if (!world.IsAlive(entityId)) {
			return;
		}

		auto* active = world.GetComponent<ActiveComponent>(entityId);
		if (active && !active->Active) {
			return;
		}

		auto* meshRenderer = world.GetComponent<MeshRendererComponent>(entityId);
		auto* localToWorld = world.GetComponent<LocalToWorldComponent>(entityId);

		if (meshRenderer && localToWorld && !meshRenderer->MeshId.empty() && !meshRenderer->MaterialId.empty()) {
			auto& resourceManager = Engine::Instance()->GetResourceManager();
			auto* provider = &Engine::Instance()->GetEngineProvider();
			Material* material = resourceManager.GetOrCreateResource<Material>(meshRenderer->MaterialId, provider);
			Mesh* mesh = resourceManager.GetResource<Mesh>(meshRenderer->MeshId);
			if (material && mesh) {
				material->PushDraw(renderBatch);
				renderBatch.PushRenderCmdToQueue(
					std::make_unique<UniformRenderCmd<Mat4>>("ModelMatrix", localToWorld->Matrix, UniformType::MAT4));
				renderBatch.PushRenderCmdToQueue(std::make_unique<MeshRenderCmd>(mesh->GetID()));
			}
		}

		auto* hierarchy = world.GetComponent<HierarchyComponent>(entityId);
		if (hierarchy) {
			for (EntityId childId : hierarchy->ChildrenIds) {
				PushDrawRecursive(world, scene, renderBatch, childId);
			}
		}
	}

	void MeshRenderSystem::Execute(World& world, Scene& scene, RenderBatch& renderBatch) {
		// Find root entities (no parent or parent not in world) and traverse from there.
		auto* hierarchyStorage = world.GetStorage<HierarchyComponent>();
		auto* transformStorage = world.GetStorage<TransformComponent>();
		if (!transformStorage) {
			return;
		}

		for (auto [entityId, _] : *transformStorage) {
			if (!world.IsAlive(entityId)) {
				continue;
			}
			EntityId parentId = NullEntity;
			if (hierarchyStorage) {
				if (auto* hierarchy = hierarchyStorage->Get(entityId)) {
					parentId = hierarchy->ParentId;
				}
			}
			// Root: no parent or parent not alive
			if (parentId == NullEntity || !world.IsAlive(parentId)) {
				PushDrawRecursive(world, scene, renderBatch, entityId);
			}
		}
	}

}
