#pragma once

#include "Core.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"

namespace Iberus {
	class IBERUS_API Engine {
	public:
		static Engine* Instance();

		void Boot();

		void Update();

		Renderer& GetRenderer() { return *renderer.get(); }

		ResourceManager& GetResourceManager() { return *resourceManager.get(); }

		SceneManager& GetSceneManager() { return *sceneManager.get(); }

	private:
		std::unique_ptr<Renderer> renderer;
		std::unique_ptr<ResourceManager> resourceManager;
		std::unique_ptr<SceneManager> sceneManager;
	};
}



