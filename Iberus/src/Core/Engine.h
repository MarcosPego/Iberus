#pragma once

#include "Core.h"
#include "RenderCmd.h"
#include "RenderSettings.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "InputManager.h"

#include "FileSystemProvider.h"
#include "ScriptHost.h"

#include <vector>

namespace Iberus {
	class Window;

	class IBERUS_API Engine {
	public:
		static Engine* Instance();

		void Boot();

		void Update();

		void OnWindowResize(uint32_t width, uint32_t height);

		Renderer& GetRenderer() { return *renderer.get(); }

		PostProcessSettings& GetPostProcessSettings() { return postProcessSettings; }
		const PostProcessSettings& GetPostProcessSettings() const { return postProcessSettings; }

		ResourceManager& GetResourceManager() { return *resourceManager.get(); }

		SceneManager& GetSceneManager() { return *sceneManager.get(); }

		InputManager& GetInputManager() { return *inputManager.get(); }

		FileSystemProvider& GetEngineProvider() { return *engineProvider.get(); }

		ScriptHost& GetScriptHost() { return *scriptHost.get(); }

		void SetCurrentWindow(Window* window);
		Window* GetCurrentWindow() const;

		/// Single viewport mode (legacy). Prefer AddEditorView for multi-view.
		void SetEditorRenderTarget(unsigned int fboId, int width, int height);
		void ClearEditorRenderTarget();
		bool HasEditorRenderTarget() const;

		/// Multi-viewport: add a view to render this frame. Camera null = use scene active camera.
		void AddEditorView(unsigned int fboId, int width, int height, std::unique_ptr<CameraRenderCmd> camera);

		void OnSwitchedToGameMode();

		/// Sync renderer to current effective output size (viewport in Editor, window in Game).
		void SyncRendererToOutput();

		void SetSceneSimulationEnabled(bool enabled);
		bool IsSceneSimulationEnabled() const { return sceneSimulationEnabled; }
		void RequestStepSimulation();
		bool ConsumeStepRequest();

		/// Script base dir: project root when a project is open, else exe dir. Used to resolve script assembly paths.
		void SetScriptBaseDir(const std::string& dir) { scriptBaseDir = dir; }
		const std::string& GetScriptBaseDir() const { return scriptBaseDir; }

		/// Scans projectRoot/Assets/Scripts for *.csproj, copies Iberus.Scripts.dll, and builds each project.
		void BuildProjectScripts(const std::string& projectRoot);

		/// Deletes projectRoot/Assets/Scripts/ScriptsContext. Call after UnloadAll() so the run folder is released.
		void DeleteProjectScriptsContext(const std::string& projectRoot);

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

		struct EditorView {
			unsigned int fbo{ 0 };
			int width{ 0 };
			int height{ 0 };
			std::unique_ptr<CameraRenderCmd> camera;
		};
		std::vector<EditorView> editorViews;
		bool sceneSimulationEnabled{ false };
		bool stepSimulationRequested{ false };
		std::string scriptBaseDir;

		PostProcessSettings postProcessSettings;
	};
}



