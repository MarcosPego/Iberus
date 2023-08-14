#include "Enginepch.h"
#include "Engine.h"

#include "FileSystem.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"

#define USE_DEFERRED 0

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

		if (USE_DEFERRED) {
			renderer = std::unique_ptr<Renderer>(Renderer::CreateDeferred());
			SetupDeferredRenderer();
		}
		else {
			renderer = std::unique_ptr<Renderer>(Renderer::Create());
		}
	
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
		
		/// Textures reserved for geometry pass
		resourceManager->CreateResource<Texture>("worldPosOut", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("diffuseOut", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("normalOut", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
		resourceManager->CreateResource<Texture>("uvsOut", currentWindow->GetWidth(), currentWindow->GetHeight(), 4);
	}
}