#pragma once

#include "Core.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "InputManager.h"

#include "FileSystemProvider.h"

namespace Iberus {
	class Window;

	class IBERUS_API Engine {
	public:
		static Engine* Instance();

		void Boot();

		void Update();

		void OnWindowResize(uint32_t width, uint32_t height);

		Renderer& GetRenderer() { return *renderer.get(); }

		ResourceManager& GetResourceManager() { return *resourceManager.get(); }

		SceneManager& GetSceneManager() { return *sceneManager.get(); }

		InputManager& GetInputManager() { return *inputManager.get(); }

		FileSystemProvider& GetEngineProvider() { return *engineProvider.get(); }

		void SetCurrentWindow(Window* window);
		Window* GetCurrentWindow() const;

		void SetEditorRenderTarget(unsigned int fboId, int width, int height);
		void ClearEditorRenderTarget();

		/// Returns effective render dimensions (editor viewport when active, else main window).
		int GetEffectiveRenderWidth() const;
		int GetEffectiveRenderHeight() const;
		float GetEffectiveRenderAspectRatio() const;

	private:
		void SetupDeferredRenderer();
		void SetupForwardRenderer();

		Window* currentWindow{ nullptr };

		std::unique_ptr<Renderer> renderer;
		std::unique_ptr<ResourceManager> resourceManager;
		std::unique_ptr<SceneManager> sceneManager;
		std::unique_ptr<InputManager> inputManager;

		std::unique_ptr<FileSystemProvider> engineProvider;

		unsigned int editorRenderTargetFBO{ 0 };
		int editorRenderTargetWidth{ 0 };
		int editorRenderTargetHeight{ 0 };
		bool editorRenderTargetPendingResize{ false };
		int pendingResizeWidth{ 0 };
		int pendingResizeHeight{ 0 };
	};
}



