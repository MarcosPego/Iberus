#include "Enginepch.h"
#include "Engine.h"

#include "FileSystem.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"

#include "MeshFactory.h"
#include "ScriptHost.h"
#include "BehaviourSystem.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>

#define USE_DEFERRED 1

namespace Iberus {

	Engine* Engine::Instance() {
		static Engine engine;
		return &engine;
	}

	void Engine::BuildProjectScripts(const std::string& projectRoot) {
		std::filesystem::path scriptsDir = std::filesystem::path(projectRoot) / "Assets" / "Scripts";
		if (!std::filesystem::exists(scriptsDir) || !std::filesystem::is_directory(scriptsDir)) {
			return;
		}
		std::string exeDir = FileSystem::GetExeDirectory();
		std::filesystem::path gameScriptsSrc = std::filesystem::path(exeDir) / "Game.Scripts.dll";
		std::filesystem::path gameScriptsDst = scriptsDir / "Game.Scripts.dll";
		if (std::filesystem::exists(gameScriptsSrc)) {
			std::filesystem::copy_file(gameScriptsSrc, gameScriptsDst, std::filesystem::copy_options::overwrite_existing);
		}
		for (const auto& entry : std::filesystem::directory_iterator(scriptsDir)) {
			if (!entry.is_regular_file() || entry.path().extension() != ".csproj") {
				continue;
			}
			std::string csprojPath = entry.path().string();
			std::string cmd = "dotnet build \"" + csprojPath + "\" -c Release -nologo -v q 2>nul";
			std::system(cmd.c_str());
		}
	}

	void Engine::Boot() {
		resourceManager = std::make_unique<ResourceManager>();
		sceneManager = std::make_unique<SceneManager>();
		inputManager = std::make_unique<InputManager>();
		engineProvider = std::make_unique<FileSystemProvider>();
		engineProvider->SetWorkingDir(FileSystem::GetWorkingDir());

		scriptHost = std::make_unique<ScriptHost>();
		std::string exeDir = FileSystem::GetExeDirectory();
		std::string scriptsConfig = exeDir + "/Game.Scripts.runtimeconfig.json";
		bool configExists = std::filesystem::exists(scriptsConfig);
		bool nethostExists = std::filesystem::exists(exeDir + "/nethost.dll");
		bool dllExists = std::filesystem::exists(exeDir + "/Game.Scripts.dll");
		IB_CORE_INFO("[Scripts] Exe dir: {} | runtimeconfig.json: {} | nethost.dll: {} | Game.Scripts.dll: {}",
			exeDir, configExists, nethostExists, dllExists);
		if (configExists) {
			if (!scriptHost->Initialize(scriptsConfig)) {
				IB_CORE_WARN("ScriptHost: Initialize failed (check nethost.dll and .NET runtime). C# scripts will not run.");
			}
		} else {
			IB_CORE_WARN("ScriptHost: {} not found. Build Game project to generate C# output.", scriptsConfig);
		}

#ifdef USE_DEFERRED // Use deferred Pipeline
			renderer = std::unique_ptr<Renderer>(Renderer::CreateDeferred());
			SetupDeferredRenderer();
			renderer->Init();
#else // Use forward Pipeline
			renderer = std::unique_ptr<Renderer>(Renderer::Create());
			SetupForwardRenderer();
			renderer->Init();
#endif

		SyncRendererToOutput();
	}

	void Engine::Update() {
		using Clock = std::chrono::high_resolution_clock;
		static auto lastFrameTime = Clock::now();

		auto now = Clock::now();
		double deltaTime = std::chrono::duration<double>(now - lastFrameTime).count();
		lastFrameTime = now;

		auto frame = Frame();
		auto* scene = sceneManager->GetActiveScene();
		if (scene) {
			if (sceneSimulationEnabled) {
				scene->Update(deltaTime);
			}
			scene->PushDraw(frame, GetCameraOverride());
			scene->PushDrawSDF(frame);
		}

		int renderWidth = currentWindow->GetWidth();
		int renderHeight = currentWindow->GetHeight();
		unsigned int outputFBO = 0;
		if (editorRenderTargetFBO != 0 && editorRenderTargetWidth > 0 && editorRenderTargetHeight > 0) {
			renderWidth = editorRenderTargetWidth;
			renderHeight = editorRenderTargetHeight;
			outputFBO = editorRenderTargetFBO;
		}
		frame.renderWidth = renderWidth;
		frame.renderHeight = renderHeight;
		renderer->RenderFrame(frame, outputFBO, renderWidth, renderHeight);
		inputManager->OnFrameEnd();
	}

	void Engine::SetEditorRenderTarget(unsigned int fboId, int width, int height) {
		editorRenderTargetFBO = fboId;
		if (width != editorRenderTargetWidth || height != editorRenderTargetHeight) {
			pendingResizeWidth = width;
			pendingResizeHeight = height;
			editorRenderTargetPendingResize = true;
		}
		editorRenderTargetWidth = width;
		editorRenderTargetHeight = height;
	}

	void Engine::ClearEditorRenderTarget() {
		editorRenderTargetFBO = 0;
		editorRenderTargetWidth = 0;
		editorRenderTargetHeight = 0;
	}

	bool Engine::HasEditorRenderTarget() const {
		return editorRenderTargetFBO != 0 && editorRenderTargetWidth > 0 && editorRenderTargetHeight > 0;
	}

	void Engine::SetSceneSimulationEnabled(bool enabled) {
		if (sceneSimulationEnabled && !enabled && scriptHost && scriptHost->IsInitialized()) {
			scriptHost->UnloadAll();
			BehaviourSystem::ClearScriptHandles();
		}
		if (enabled && !sceneSimulationEnabled) {
			IB_CORE_INFO("[Scripts] Game mode started (F11) - scene simulation and C# scripts now run.");
		}
		sceneSimulationEnabled = enabled;
	}

	void Engine::OnSwitchedToGameMode() {
		if (scriptHost && scriptHost->IsInitialized()) {
			scriptHost->UnloadAll();
			BehaviourSystem::ClearScriptHandles();
		}
		ClearEditorRenderTarget();
		SyncRendererToOutput();
	}

	void Engine::SyncRendererToOutput() {
		int w = GetEffectiveRenderWidth();
		int h = GetEffectiveRenderHeight();
		if (renderer && w > 0 && h > 0) {
			renderer->Resize(w, h);
		}
	}

	void Engine::SetCameraOverride(std::unique_ptr<CameraRenderCmd> cmd) {
		cameraOverride = std::move(cmd);
	}

	int Engine::GetEffectiveRenderWidth() const {
		if (editorRenderTargetFBO != 0 && editorRenderTargetWidth > 0 && editorRenderTargetHeight > 0) {
			return editorRenderTargetWidth;
		}
		return currentWindow ? currentWindow->GetWidth() : 0;
	}

	int Engine::GetEffectiveRenderHeight() const {
		if (editorRenderTargetFBO != 0 && editorRenderTargetWidth > 0 && editorRenderTargetHeight > 0) {
			return editorRenderTargetHeight;
		}
		return currentWindow ? currentWindow->GetHeight() : 0;
	}

	float Engine::GetEffectiveRenderAspectRatio() const {
		int w = GetEffectiveRenderWidth();
		int h = GetEffectiveRenderHeight();
		if (w <= 0 || h <= 0) {
			return 16.0f / 9.0f;
		}
		return static_cast<float>(w) / static_cast<float>(h);
	}

	void Engine::SetCurrentWindow(Window* window) {
		currentWindow = window;
	}

	void Engine::OnWindowResize(uint32_t width, uint32_t height) {
		(void)width;
		(void)height;
		SyncRendererToOutput();
	}

	Window* Engine::GetCurrentWindow() const  {
		return currentWindow;
	}

	void Engine::SetupDeferredRenderer() {
		/// Init necessary deferred renderer resources
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseGeometryShader", engineProvider.get());
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseDeferredLightShader", engineProvider.get());
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseRaymarchingShader", engineProvider.get());
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseHDRShader", engineProvider.get());
		
		/// Textures reserved for passes 1
		resourceManager->CreateResource<Texture>("worldPosOut_1", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("diffuseOut_1", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("normalOut_1", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("uvsOut_1", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);

		/// Textures reserved for passes 2
		resourceManager->CreateResource<Texture>("worldPosOut_2", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("diffuseOut_2", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("normalOut_2", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("uvsOut_2", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);

		/// Warning! This does nothing for now
		/*renderer->PushRenderCmd(new ShaderRenderCmd("assets/shaders/baseDeferredLightShader"));
		renderer->PushRenderCmd(new UniformRenderCmd("worldPosOut", 0, UniformType::INT));
		renderer->PushRenderCmd(new UniformRenderCmd("diffuseOut", 1, UniformType::INT));
		renderer->PushRenderCmd(new UniformRenderCmd("normalOut", 2, UniformType::INT));
		renderer->PushRenderCmd(new UniformRenderCmd("uvsOut", 3, UniformType::INT));*/

		MeshFactory::CreateQuad("renderQuad", *resourceManager, currentWindow->GetWidth(), currentWindow->GetHeight());
		MeshFactory::CreateNDCQuad("renderQuadNDC", *resourceManager);

		renderer->ExecuteAndFlushCmdQueue();
	}

	void Engine::SetupForwardRenderer() {
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseShader", engineProvider.get());
		renderer->ExecuteAndFlushCmdQueue();
	}
}