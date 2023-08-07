#include "Enginepch.h"
#include "Scene.h"

#include "Camera.h"
#include "Entity.h"
#include "Renderer.h"
#include "RenderCmd.h"

namespace Iberus {
	void Scene::Update(double deltaTime) {

	}

	void Scene::PushDraw(Frame& frame) {
		/// For render targets
		auto& renderBatch = frame.PushBatch();

		renderBatch.PushRenderCmd(new CameraRenderCmd(activeCamera->GetViewMatrix(), activeCamera->GetProjectionMatrix()));
		if (sceneRoot) {
			sceneRoot->PushDraw(renderBatch);
		}
	}
}