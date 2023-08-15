#include "Enginepch.h"
#include "Engine.h"

#include "FileSystem.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"

#include "MeshFactory.h"

#define USE_DEFERRED 1

namespace Iberus {

	Engine* Engine::Instance() {
		static Engine engine;
		return &engine;
	}

	void Engine::Boot() {
		resourceManager = std::make_unique<ResourceManager>();
		sceneManager = std::make_unique<SceneManager>();
		engineProvider = std::make_unique<FileSystemProvider>();
		engineProvider->SetWorkingDir(FileSystem::GetWorkingDir());

#ifdef USE_DEFERRED // Use deferred Pipeline
			renderer = std::unique_ptr<Renderer>(Renderer::CreateDeferred());
			SetupDeferredRenderer();
			renderer->Init();
#else // Use forward Pipeline
			renderer = std::unique_ptr<Renderer>(Renderer::Create());
			SetupForwardRenderer();
			renderer->Init();
#endif
	
	}

	void Engine::Update() {
		auto frame = Frame();
		auto* scene = sceneManager->GetActiveScene();
		scene->Update(0);
		scene->PushDraw(frame);
		renderer->RenderFrame(frame);
	}

	void Engine::SetCurrentWindow(Window* window) {
		currentWindow = window;
	}

	Window* Engine::GetCurrentWindow() const  {
		return currentWindow;
	}

	void Engine::SetupDeferredRenderer() {
		/// Init necessary deferred renderer resources
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseGeometryShader", engineProvider.get());
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseDeferredLightShader", engineProvider.get());
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseRaymarchingShader", engineProvider.get());
		
		/// Textures reserved for geometry pass
		resourceManager->CreateResource<Texture>("worldPosOut", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("diffuseOut", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("normalOut", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("uvsOut", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);

		/// Warning! This does nothing for now
		renderer->PushRenderCmd(new ShaderRenderCmd("assets/shaders/baseDeferredLightShader"));
		renderer->PushRenderCmd(new UniformRenderCmd("worldPosOut", 0, UniformType::INT));
		renderer->PushRenderCmd(new UniformRenderCmd("diffuseOut", 1, UniformType::INT));
		renderer->PushRenderCmd(new UniformRenderCmd("normalOut", 2, UniformType::INT));
		renderer->PushRenderCmd(new UniformRenderCmd("uvsOut", 3, UniformType::INT));

		MeshFactory::CreateQuad("renderQuad", *resourceManager, currentWindow->GetWidth(), currentWindow->GetHeight());

		renderer->ExecuteAndFlushCmdQueue();
	}

	void Engine::SetupForwardRenderer() {
		resourceManager->GetOrCreateResource<Shader>("assets/shaders/baseShader", engineProvider.get());
		renderer->ExecuteAndFlushCmdQueue();
	}
}