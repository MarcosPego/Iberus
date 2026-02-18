#include "Enginepch.h"
#include "Scene.h"

#include "Renderer.h"
#include "RenderCmd.h"
#include "RenderBatch.h"
#include <algorithm>
#include "Components.h"
#include "TransformSystem.h"
#include "CameraSystem.h"
#include "LightSystem.h"
#include "MeshRenderSystem.h"
#include "TerrainSystem.h"
#include "SDFRenderSystem.h"
#include "BehaviourSystem.h"

namespace Iberus {
	Scene::Scene(const std::string& inID) {
		ID = inID;

		sceneRootId = CreateEntityECS("__rootEntity");
		world.AddComponent<TransformComponent>(sceneRootId);
		world.AddComponent<HierarchyComponent>(sceneRootId);
		world.AddComponent<TagComponent>(sceneRootId, "__rootEntity", "Scene Root");
		world.AddComponent<ActiveComponent>(sceneRootId, true);

		activeCameraId = CreateEntityECS("Camera");
		world.AddComponent<TransformComponent>(activeCameraId);
		world.AddComponent<HierarchyComponent>(activeCameraId);
		world.AddComponent<TagComponent>(activeCameraId, "Camera", "Camera");
		world.AddComponent<ActiveComponent>(activeCameraId, true);
		world.AddComponent<CameraComponent>(activeCameraId);

		AddChildECS(sceneRootId, activeCameraId, "Camera");

		EntityId defaultLightId = CreateEntityECS("Light");
		world.AddComponent<TransformComponent>(defaultLightId)->Position = Vec3(15, 15, 25);
		world.AddComponent<LightComponent>(defaultLightId);
		AddChildECS(sceneRootId, defaultLightId, "Light");
	}

	EntityId Scene::CreateEntityECS(const std::string& id) {
		EntityId e = world.CreateEntity();
		world.AddComponent<TagComponent>(e, id, id);
		world.AddComponent<TransformComponent>(e);
		world.AddComponent<HierarchyComponent>(e);
		world.AddComponent<ActiveComponent>(e, true);
		return e;
	}

	void Scene::AddChildECS(EntityId parentId, EntityId childId, const std::string& childTagId) {
		(void)childTagId;
		auto* hierarchy = world.GetComponent<HierarchyComponent>(childId);
		if (hierarchy) {
			hierarchy->ParentId = parentId;
		}
		auto* parentHierarchy = world.GetComponent<HierarchyComponent>(parentId);
		if (parentHierarchy) {
			parentHierarchy->ChildrenIds.push_back(childId);
		}
	}

	void Scene::RemoveChildFromParent(EntityId parentId, EntityId childId) {
		auto* parentHierarchy = world.GetComponent<HierarchyComponent>(parentId);
		if (parentHierarchy) {
			auto& children = parentHierarchy->ChildrenIds;
			children.erase(std::remove(children.begin(), children.end(), childId), children.end());
		}
		auto* childHierarchy = world.GetComponent<HierarchyComponent>(childId);
		if (childHierarchy) {
			childHierarchy->ParentId = NullEntity;
		}
	}

	void Scene::DestroyEntityWithDescendants(EntityId entityId) {
		if (entityId == NullEntity || !world.IsAlive(entityId)) {
			return;
		}
		auto* hierarchy = world.GetComponent<HierarchyComponent>(entityId);
		EntityId parentId = hierarchy ? hierarchy->ParentId : NullEntity;

		// Remove from parent first so we're no longer reachable from hierarchy traversal
		if (parentId != NullEntity) {
			RemoveChildFromParent(parentId, entityId);
		}

		// Recursively destroy children, then self
		std::vector<EntityId> children;
		if (hierarchy) {
			children = hierarchy->ChildrenIds;
			hierarchy->ParentId = NullEntity;
			hierarchy->ChildrenIds.clear();
		}
		for (EntityId childId : children) {
			DestroyEntityWithDescendants(childId);
		}
		world.DestroyEntity(entityId);
	}

	void Scene::QueueDestroyEntity(EntityId entityId) {
		if (entityId != NullEntity && entityId != sceneRootId) {
			pendingDestroys.push_back(entityId);
		}
	}

	void Scene::FlushPendingDestroys() {
		for (EntityId entityId : pendingDestroys) {
			DestroyEntityWithDescendants(entityId);
		}
		pendingDestroys.clear();
	}

		std::vector<std::string> Scene::GetBehavioursForEntity(EntityId entityId) const {
			std::vector<std::string> types;
			for (const auto& [typeName, list] : registeredBehaviours) {
				for (const auto& [eid, _] : list) {
					if (eid == entityId) {
						types.push_back(typeName);
						break;
					}
				}
			}
			return types;
		}

		void Scene::ClearRegisteredBehaviours() {
			registeredBehaviours.clear();
		}

		std::string Scene::GenerateUniqueTagId(const std::string& base) {
		std::string candidate = base;
		int suffix = 1;
		auto* tagStorage = world.GetStorage<TagComponent>();
		if (!tagStorage) {
			return candidate;
		}
		while (true) {
			bool found = false;
			for (auto [eid, tag] : *tagStorage) {
				if (tag.Id == candidate) {
				found = true;
				break;
			}
			}
			if (!found) {
				return candidate;
			}
			candidate = base + "_" + std::to_string(suffix++);
		}
	}

	static EntityId CloneEntityRecursive(Scene& scene, EntityId sourceId, EntityId newParentId, const std::string& parentTagForChild) {
		(void)parentTagForChild;
		World& world = scene.GetWorld();
		if (sourceId == NullEntity || !world.IsAlive(sourceId)) {
			return NullEntity;
		}

		auto* srcTag = world.GetComponent<TagComponent>(sourceId);
		std::string newTagId = scene.GenerateUniqueTagId(srcTag ? srcTag->Id + "_clone" : "Entity_clone");
		EntityId newId = scene.CreateEntityECS(newTagId);

		if (auto* srcT = world.GetComponent<TransformComponent>(sourceId)) {
			if (auto* dstT = world.GetComponent<TransformComponent>(newId)) *dstT = *srcT;
		}
		if (auto* srcA = world.GetComponent<ActiveComponent>(sourceId)) {
			if (auto* dstA = world.GetComponent<ActiveComponent>(newId)) *dstA = *srcA;
		}
		if (auto* srcM = world.GetComponent<MeshRendererComponent>(sourceId)) {
			MeshRendererComponent copy = *srcM;
			*scene.AddComponent<MeshRendererComponent>(newId) = copy;
		}
		if (auto* srcC = world.GetComponent<CameraComponent>(sourceId)) {
			CameraComponent copy = *srcC;
			*scene.AddComponent<CameraComponent>(newId) = copy;
		}
		if (auto* srcL = world.GetComponent<LightComponent>(sourceId)) {
			LightComponent copy = *srcL;
			*scene.AddComponent<LightComponent>(newId) = copy;
		}
		if (auto* srcS = world.GetComponent<SDFComponent>(sourceId)) {
			SDFComponent copy = *srcS;
			*scene.AddComponent<SDFComponent>(newId) = copy;
		}
		if (auto* srcT = world.GetComponent<TerrainComponent>(sourceId)) {
			TerrainComponent copy = *srcT;
			copy.NeedsRegenerate = true;
			*scene.AddComponent<TerrainComponent>(newId) = copy;
		}
		if (auto* srcSc = world.GetComponent<ScriptComponent>(sourceId)) {
			ScriptComponent copy = *srcSc;
			*scene.AddComponent<ScriptComponent>(newId) = copy;
		}

		// Copy children list before recursing to avoid iterator invalidation if storage reallocates
		std::vector<EntityId> childIds;
		if (auto* srcHierarchy = world.GetComponent<HierarchyComponent>(sourceId)) {
			childIds = srcHierarchy->ChildrenIds;
		}

		if (newParentId != NullEntity) {
			scene.AddChildECS(newParentId, newId, newTagId);
		}

		for (EntityId childId : childIds) {
			CloneEntityRecursive(scene, childId, newId, newTagId);
		}
		return newId;
	}

	EntityId Scene::CloneEntityWithDescendants(EntityId sourceId) {
		return CloneEntityRecursive(*this, sourceId, NullEntity, "");
	}

	void Scene::Update(double deltaTime) {
		BehaviourSystem::Update(world, *this, deltaTime);
	}

	void Scene::PushDraw(Frame& frame, CameraRenderCmd* camera) {
		auto& renderBatch = frame.PushBatch();

		TransformSystem::Update(world);
		TerrainSystem::Update(world);
		if (camera) {
			renderBatch.PushRenderCmdToQueue(
				std::make_unique<CameraRenderCmd>(
					camera->viewMatrix,
					camera->projectionMatrix,
					camera->cameraPos,
					camera->cameraToWorld),
				CMDQueue::Camera);
		} else {
			CameraSystem::Execute(world, renderBatch);
		}
		LightSystem::Execute(world, renderBatch);
		MeshRenderSystem::Execute(world, *this, renderBatch);
	}

	void Scene::PushDrawSDF(Frame& frame) {
		for (auto& batch : frame.renderBatches) {
			SDFRenderSystem::Execute(world, *this, batch);
		}
	}
}