#include "Enginepch.h"
#include "Application.h"

namespace Iberus {
	Application::Application() {
		static WindowProps winProps{};
		window = std::unique_ptr<Window>(Window::Create(winProps));
	}

	Application::~Application() {
	}

	void Application::Run() {
		while (running) {
			window->Update();
		}
	}
}