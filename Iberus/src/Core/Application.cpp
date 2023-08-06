#include "Enginepch.h"
#include "Application.h"

#include "RenderCmd.h"

#include "Renderer.h"

namespace Iberus {
	Application::Application() {
		static WindowProps winProps{};
		window = std::unique_ptr<Window>(Window::Create(winProps));
		renderer = std::unique_ptr<Renderer>(Renderer::Create());
	}

	Application::~Application() {
	}

	void Application::Run() {
		while (running) {
			auto frame = Frame();

			// test case
			/*auto& renderBatch = frame.PushBatch();
			auto uniform = new UniformRenderCmd<int>("test", 2, UniformType::INT);
			renderBatch.PushRenderCmd(uniform);*/


			renderer->RenderFrame(frame);
			window->Update();
		}
	}
}