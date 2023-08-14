#include "Enginepch.h"
#include "Scene.h"

#include "Renderer.h"
#include "RenderCmd.h"

namespace Iberus {
	Scene::Scene(const std::string& inID) {
		ID = inID;

		sceneRoot = CreateEntity<Entity>("__rootEntity");

		activeCamera = CreateEntity<Camera>("__camera");

		sceneRoot->AddEntity("__camera", activeCamera);
	}

	void Scene::AddEntity(const std::string& id, Entity* entity) {
		sceneRoot->AddEntity(id, entity);
	}

	void Scene::Update(double deltaTime) {

	}

	void Scene::PushDraw(Frame& frame) {
		/// For render targets
		auto& renderBatch = frame.PushBatch();

		if (activeCamera) {
			renderBatch.PushCameraRenderCmd(new CameraRenderCmd(activeCamera->GetViewMatrix(), activeCamera->GetProjectionMatrix()));
		}
		
		if (sceneRoot) {
			sceneRoot->PushDraw(renderBatch);
		}
	}
}