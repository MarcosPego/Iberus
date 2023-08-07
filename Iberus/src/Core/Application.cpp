#include "Enginepch.h"
#include "Application.h"

#include "RenderCmd.h"

#include "Renderer.h"
#include "Engine.h"

namespace Iberus {
	Application::Application() {
		static WindowProps winProps{};
		window = std::unique_ptr<Window>(Window::Create(winProps));
		engine = Engine::Instance();
	}

	Application::~Application() {
	}

	void Application::Boot() {
		engine->Boot();
	}

	void Application::Update() {
	}

	void Application::Run() {
		while (running) {


			// test case
			/*auto& renderBatch = frame.PushBatch();
			auto uniform = new UniformRenderCmd<int>("test", 2, UniformType::INT);
			renderBatch.PushRenderCmd(uniform);*/

			engine->Update();		
			Update();

			window->Update();
		}
	}
}