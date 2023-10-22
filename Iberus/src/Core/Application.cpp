#include "Enginepch.h"
#include "Application.h"

#include "RenderCmd.h"

#include "Renderer.h"
#include "Engine.h"

namespace Iberus {

#define BIND_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application::Application() {
		static WindowProps winProps{};
		window = std::unique_ptr<Window>(Window::Create(winProps));
		window->SetEventCallback(BIND_FN(Application::OnEvent));
		engine = Engine::Instance();
		engine->SetCurrentWindow(window.get());
	}

	Application::~Application() {
	}

	void Application::Boot() {
		engine->Boot();
	}

	void Application::OnEvent(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_FN(Application::OnWindowClose));

		IB_CORE_TRACE("{}", event.ToString());
	}

	bool Application::OnWindowClose(WindowCloseEvent& event) {
		running = false;
		return true;
	}

	void Application::Update() {
	}

	void Application::Run() {
		while (running) {
			engine->Update();		
			Update();
			window->Update();
		}
	}
}