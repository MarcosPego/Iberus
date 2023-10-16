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

		if (auto* sdfEntity = dynamic_cast<SDFEntity*>(entity); sdfEntity) {
			sdfEntities.push_back(sdfEntity);
		}
	}

	void Scene::Update(double deltaTime) {
		for (auto& [behaviourType, behaviours] : registeredBehaviours) {
			for (auto& [entity, behaviour] : behaviours) {
				if (entity->GetActive()) {
					behaviour->Update(deltaTime);
				}
			}
		}
	}

	void Scene::PushDraw(Frame& frame) {
		/// For render targets
		auto& renderBatch = frame.PushBatch();

		if (activeCamera) {
			renderBatch.PushRenderCmdToQueue(new CameraRenderCmd(activeCamera->GetViewMatrix(), activeCamera->GetProjectionMatrix(), activeCamera->GetPosition(),
				activeCamera->GetCameraToWorld()			
			), CMDQueue::Camera);
		}
		
		if (sceneRoot) {
			sceneRoot->PushDraw(renderBatch);
		}
	}

	void Scene::PushDrawSDF(Frame& frame) {
		// TODO(MPP) There is a maximum for sdf enetities;
		
		for (auto& renderBatch : frame.renderBatches) {

			int count = 0;
			for (const auto& entity : sdfEntities) {
				entity->PushDrawSDF(renderBatch, count);
				count++;
			}
		}

	}
}