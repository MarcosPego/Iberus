#pragma once

#include "Core.h"
#include "RenderCmd.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "InputManager.h"

#include "FileSystemProvider.h"
#include "ScriptHost.h"

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

		ScriptHost& GetScriptHost() { return *scriptHost.get(); }

		void SetCurrentWindow(Window* window);
		Window* GetCurrentWindow() const;

		void SetEditorRenderTarget(unsigned int fboId, int width, int height);
		void ClearEditorRenderTarget();
		bool HasEditorRenderTarget() const;
		void OnSwitchedToGameMode();

		/// Sync renderer to current effective output size (viewport in Editor, window in Game).
		void SyncRendererToOutput();

		/// When set, render uses this camera instead of scene's active camera. Target and camera are set by the orchestration layer.
		void SetCameraOverride(std::unique_ptr<CameraRenderCmd> cmd);
		CameraRenderCmd* GetCameraOverride() { return cameraOverride.get(); }

		void SetSceneSimulationEnabled(bool enabled);
		bool IsSceneSimulationEnabled() const { return sceneSimulationEnabled; }

		/// Script base dir: project root when a project is open, else exe dir. Used to resolve script assembly paths.
		void SetScriptBaseDir(const std::string& dir) { scriptBaseDir = dir; }
		const std::string& GetScriptBaseDir() const { return scriptBaseDir; }

		/// Scans projectRoot/Assets/Scripts for *.csproj, copies Iberus.Scripts.dll, and builds each project.
		void BuildProjectScripts(const std::string& projectRoot);

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
		std::unique_ptr<ScriptHost> scriptHost;

		unsigned int editorRenderTargetFBO{ 0 };
		int editorRenderTargetWidth{ 0 };
		int editorRenderTargetHeight{ 0 };
		bool editorRenderTargetPendingResize{ false };
		int pendingResizeWidth{ 0 };
		int pendingResizeHeight{ 0 };

		std::unique_ptr<CameraRenderCmd> cameraOverride;
		bool sceneSimulationEnabled{ false };
		std::string scriptBaseDir;
	};
}



