#include "Enginepch.h"
#include "Engine.h"

namespace Iberus {

	Engine* Engine::Instance() {
		static Engine engine;
		return &engine;
	}

	void Engine::Boot() {
		renderer = std::unique_ptr<Renderer>(Renderer::Create());
		resourceManager = std::make_unique<ResourceManager>();
	}

	void Engine::Update() {
		auto frame = Frame();
		renderer->RenderFrame(frame);
	}
}