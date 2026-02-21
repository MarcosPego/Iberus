#include "Enginepch.h"
#include "Engine.h"

#include "FileSystem.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Profiler.h"

#include "MeshFactory.h"
#include "ScriptHost.h"
#include "BehaviourSystem.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <thread>

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
		std::filesystem::path scriptsContextDir = scriptsDir / "ScriptsContext";
		std::filesystem::path runDir = scriptsContextDir / "run";
		std::filesystem::create_directories(scriptsContextDir);
		std::filesystem::create_directories(runDir);

		std::string appDir = FileSystem::GetAppDirectory();
		std::filesystem::path scriptsSrc = std::filesystem::path(appDir) / "Iberus.Scripts.dll";
		std::filesystem::path scriptsDst = scriptsContextDir / "Iberus.Scripts.dll";
		if (std::filesystem::exists(scriptsSrc)) {
			std::filesystem::copy_file(scriptsSrc, scriptsDst, std::filesystem::copy_options::overwrite_existing);
		}

		for (const auto& entry : std::filesystem::directory_iterator(scriptsDir)) {
			if (!entry.is_regular_file() || entry.path().extension() != ".csproj") {
				continue;
			}
			std::filesystem::path csprojPath = entry.path();
			std::string assemblyName = csprojPath.stem().string();
			std::filesystem::path outputDll = scriptsContextDir / (assemblyName + ".dll");

			// Change detection: skip build if output DLL exists and is newer than all .cs and .csproj in scripts dir
			std::filesystem::file_time_type latestSource = std::filesystem::file_time_type::min();
			for (const auto& f : std::filesystem::directory_iterator(scriptsDir)) {
				if (!f.is_regular_file()) {
					continue;
				}
				std::string ext = f.path().extension().string();
				if (ext == ".cs" || (ext == ".csproj" && f.path().filename() == csprojPath.filename())) {
					auto t = std::filesystem::last_write_time(f.path());
					if (t > latestSource) {
						latestSource = t;
					}
				}
			}
			bool outputExists = std::filesystem::exists(outputDll);
			bool needBuild = !outputExists ||
				(outputExists && std::filesystem::last_write_time(outputDll) < latestSource);

			if (needBuild) {
				IB_CORE_INFO("[Scripts] Building {} (--no-incremental)", csprojPath.string());
				std::string cmd = "dotnet build \"" + csprojPath.string() + "\" -c Release --no-incremental -nologo";
				int ret = std::system(cmd.c_str());
				if (ret != 0) {
					IB_CORE_WARN("[Scripts] Build returned {} for {}", ret, csprojPath.string());
					continue;
				}
			} else {
				IB_CORE_INFO("[Scripts] Skipping build for {} (no changes)", csprojPath.string());
			}

			// Ensure run folder has latest DLLs: copy from ScriptsContext so we load from run (avoids build overwriting in-use file)
			if (std::filesystem::exists(outputDll)) {
				std::filesystem::path runDll = runDir / (assemblyName + ".dll");
				std::filesystem::copy_file(outputDll, runDll, std::filesystem::copy_options::overwrite_existing);
				std::filesystem::path depsPath = scriptsContextDir / (assemblyName + ".deps.json");
				if (std::filesystem::exists(depsPath)) {
					std::filesystem::copy_file(depsPath, runDir / (assemblyName + ".deps.json"), std::filesystem::copy_options::overwrite_existing);
				}
			}
		}

		// Copy Iberus.Scripts.dll and deps to run so runtime can load from run/
		if (std::filesystem::exists(scriptsDst)) {
			std::filesystem::path runIberus = runDir / "Iberus.Scripts.dll";
			std::filesystem::copy_file(scriptsDst, runIberus, std::filesystem::copy_options::overwrite_existing);
			std::filesystem::path iberusDeps = scriptsContextDir / "Iberus.Scripts.deps.json";
			if (std::filesystem::exists(iberusDeps)) {
				std::filesystem::copy_file(iberusDeps, runDir / "Iberus.Scripts.deps.json", std::filesystem::copy_options::overwrite_existing);
			}
		}
	}

	void Engine::DeleteProjectScriptsContext(const std::string& projectRoot) {
		if (projectRoot.empty()) {
			return;
		}
		std::filesystem::path scriptsContextDir = std::filesystem::path(projectRoot) / "Assets" / "Scripts" / "ScriptsContext";
		if (!std::filesystem::exists(scriptsContextDir) || !std::filesystem::is_directory(scriptsContextDir)) {
			return;
		}
		const int maxAttempts = 5;
		const int delayMs = 50;
		for (int attempt = 0; attempt < maxAttempts; ++attempt) {
			std::error_code ec;
			std::filesystem::remove_all(scriptsContextDir, ec);
			if (!ec) {
				return;
			}
			if (attempt < maxAttempts - 1) {
				std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
			} else {
				IB_CORE_WARN("[Scripts] Failed to delete ScriptsContext folder after {} attempts: {}", maxAttempts, ec.message());
			}
		}
	}

	void Engine::Boot() {
		resourceManager = std::make_unique<ResourceManager>();
		sceneManager = std::make_unique<SceneManager>();
		inputManager = std::make_unique<InputManager>();
		engineProvider = std::make_unique<FileSystemProvider>();
		engineProvider->SetWorkingDir(FileSystem::GetWorkingDir());

		scriptHost = std::make_unique<ScriptHost>();
		std::string appDir = FileSystem::GetAppDirectory();
		std::string scriptsConfig = appDir + "/Iberus.Scripts.runtimeconfig.json";
		bool configExists = std::filesystem::exists(scriptsConfig);
		bool nethostExists = std::filesystem::exists(appDir + "/nethost.dll");
		bool dllExists = std::filesystem::exists(appDir + "/Iberus.Scripts.dll");
		IB_CORE_INFO("[Scripts] App dir: {} | runtimeconfig.json: {} | nethost.dll: {} | Iberus.Scripts.dll: {}",
			appDir, configExists, nethostExists, dllExists);
		if (configExists) {
			if (!scriptHost->Initialize(scriptsConfig)) {
				IB_CORE_WARN("ScriptHost: Initialize failed (check nethost.dll and .NET runtime). C# scripts will not run.");
			}
		} else {
			IB_CORE_WARN("ScriptHost: {} not found. Build the solution to generate C# scripting output.", scriptsConfig);
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

		auto* scene = sceneManager->GetActiveScene();
		if (scene) {
			bool shouldUpdate = sceneSimulationEnabled || ConsumeStepRequest();
			if (shouldUpdate) {
				Profiler::Instance().RecordUpdate(deltaTime);
				{
					auto t0 = std::chrono::high_resolution_clock::now();
					scene->Update(deltaTime);
					auto t1 = std::chrono::high_resolution_clock::now();
					double updateMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
					Profiler::Instance().RecordZone("Update", updateMs);
				}
			}
		}

		const int minViewSize = 8;
		if (!editorViews.empty()) {
			for (auto& view : editorViews) {
				if (view.fbo == 0 || view.width < minViewSize || view.height < minViewSize) {
					continue;
				}
				// Clear the view FBO to avoid leftover scene content when resizing or when ImGui draws over it
				glBindFramebuffer(GL_FRAMEBUFFER, view.fbo);
				glViewport(0, 0, view.width, view.height);
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				editorRenderTargetFBO = view.fbo;
				editorRenderTargetWidth = view.width;
				editorRenderTargetHeight = view.height;
				Frame frame;
				frame.renderWidth = view.width;
				frame.renderHeight = view.height;
				if (scene) {
					scene->PushDraw(frame, view.camera.get());
					scene->PushDrawSDF(frame);
				}
				renderer->RenderFrame(frame, view.fbo, view.width, view.height);
			}
			editorViews.clear();
			editorRenderTargetFBO = 0;
			editorRenderTargetWidth = 0;
			editorRenderTargetHeight = 0;
		} else {
			Frame frame;
			if (scene) {
				scene->PushDraw(frame, nullptr);
				scene->PushDrawSDF(frame);
			}
			int renderWidth = currentWindow ? currentWindow->GetWidth() : 0;
			int renderHeight = currentWindow ? currentWindow->GetHeight() : 0;
			unsigned int outputFBO = 0;
			if (editorRenderTargetFBO != 0 && editorRenderTargetWidth > 0 && editorRenderTargetHeight > 0) {
				renderWidth = editorRenderTargetWidth;
				renderHeight = editorRenderTargetHeight;
				outputFBO = editorRenderTargetFBO;
			}
			frame.renderWidth = renderWidth;
			frame.renderHeight = renderHeight;
			renderer->RenderFrame(frame, outputFBO, renderWidth, renderHeight);
		}
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
		if (!editorViews.empty()) {
			return true;
		}
		return editorRenderTargetFBO != 0 && editorRenderTargetWidth > 0 && editorRenderTargetHeight > 0;
	}

	void Engine::AddEditorView(unsigned int fboId, int width, int height, std::unique_ptr<CameraRenderCmd> camera) {
		const int minSize = 8;
		if (fboId == 0 || width < minSize || height < minSize) {
			return;
		}
		EditorView view;
		view.fbo = fboId;
		view.width = width;
		view.height = height;
		view.camera = std::move(camera);
		editorViews.push_back(std::move(view));
	}

	void Engine::RequestStepSimulation() {
		stepSimulationRequested = true;
	}

	bool Engine::ConsumeStepRequest() {
		bool v = stepSimulationRequested;
		stepSimulationRequested = false;
		return v;
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
		// Unload scripts first to release DLL file lock, then rebuild so C# edits are picked up
		if (!scriptBaseDir.empty()) {
			scriptHost->UnloadAll();
			BehaviourSystem::ClearScriptHandles();
			BuildProjectScripts(scriptBaseDir);
		}
		SyncRendererToOutput();
	}

	void Engine::SyncRendererToOutput() {
		int w = GetEffectiveRenderWidth();
		int h = GetEffectiveRenderHeight();
		if (renderer && w > 0 && h > 0) {
			renderer->Resize(w, h);
		}
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
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseToonLightShader", engineProvider.get());
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseRaymarchingShader", engineProvider.get());
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseGeometryCopyShader", engineProvider.get());
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseOutlineShader", engineProvider.get());
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseHDRShader", engineProvider.get());
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/basePixelationShader", engineProvider.get());
		
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