#include "Enginepch.h"
#include "Scene.h"

#include "Entity.h"
#include "Renderer.h"

namespace Iberus {
	void Scene::Update(double deltaTime) {

	}

	void Scene::PushDraw(Frame& frame) {
		
		/// For render targets
		auto& renderBatch = frame.PushBatch();
		if (sceneRoot) {
			sceneRoot->PushDraw(renderBatch);
		}
	}
}