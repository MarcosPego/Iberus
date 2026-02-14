#include "Enginepch.h"
#include "Scene.h"

#include "Renderer.h"
#include "RenderCmd.h"
#include "Components.h"
#include "TransformSystem.h"
#include "CameraSystem.h"
#include "LightSystem.h"
#include "MeshRenderSystem.h"
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

		activeCameraId = CreateEntityECS("__camera");
		world.AddComponent<TransformComponent>(activeCameraId);
		world.AddComponent<HierarchyComponent>(activeCameraId);
		world.AddComponent<TagComponent>(activeCameraId, "__camera", "Camera");
		world.AddComponent<ActiveComponent>(activeCameraId, true);
		world.AddComponent<CameraComponent>(activeCameraId);

		AddChildECS(sceneRootId, activeCameraId, "__camera");

		EntityId defaultLightId = CreateEntityECS("__default_light");
		world.AddComponent<TransformComponent>(defaultLightId)->Position = Vec3(15, 15, 25);
		world.AddComponent<LightComponent>(defaultLightId);
		AddChildECS(sceneRootId, defaultLightId, "__default_light");
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

	void Scene::Update(double deltaTime) {
		BehaviourSystem::Update(world, *this, deltaTime);
	}

	void Scene::PushDraw(Frame& frame) {
		auto& renderBatch = frame.PushBatch();

		TransformSystem::Update(world);
		CameraSystem::Execute(world, renderBatch);
		LightSystem::Execute(world, renderBatch);
		MeshRenderSystem::Execute(world, *this, renderBatch);
	}

	void Scene::PushDrawSDF(Frame& frame) {
		for (auto& batch : frame.renderBatches) {
			SDFRenderSystem::Execute(world, *this, batch);
		}
	}
}