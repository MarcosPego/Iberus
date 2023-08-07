#include "Enginepch.h"
#include "Engine.h"

#include "Scene.h"
#include "FileSystem.h"

namespace Iberus {

	Engine* Engine::Instance() {
		static Engine engine;
		return &engine;
	}

	void Engine::Boot() {
		renderer = std::unique_ptr<Renderer>(Renderer::Create());
		resourceManager = std::make_unique<ResourceManager>();
		sceneManager = std::make_unique<SceneManager>();
		engineProvider = std::make_unique<FileSystemProvider>();
		engineProvider->SetWorkingDir(FileSystem::GetWorkingDir());
	}

	void Engine::Update() {
		auto frame = Frame();
		auto* scene = sceneManager->GetActiveScene();
		scene->Update(0);
		scene->PushDraw(frame);
		renderer->RenderFrame(frame);
	}
}