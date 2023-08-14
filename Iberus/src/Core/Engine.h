#pragma once

#include "Core.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"

#include "FileSystemProvider.h"

namespace Iberus {
	class Window;

	class IBERUS_API Engine {
	public:
		static Engine* Instance();

		void Boot();

		void Update();

		Renderer& GetRenderer() { return *renderer.get(); }

		ResourceManager& GetResourceManager() { return *resourceManager.get(); }

		SceneManager& GetSceneManager() { return *sceneManager.get(); }

		FileSystemProvider& GetEngineProvider() { return *engineProvider.get(); }

		void SetCurrentWindow(Window* window);
		Window* GetCurrentWindow() const;

	private:
		void SetupDeferredRenderer();
		void SetupForwardRenderer();

		Window* currentWindow{ nullptr };

		std::unique_ptr<Renderer> renderer;
		std::unique_ptr<ResourceManager> resourceManager;
		std::unique_ptr<SceneManager> sceneManager;

		std::unique_ptr<FileSystemProvider> engineProvider;
	};
}



