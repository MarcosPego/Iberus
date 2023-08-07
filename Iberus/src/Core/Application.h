#pragma once

#include "Core.h"
#include "Window.h"
#include "Renderer.h"

#include "Scene.h" // temp

namespace Iberus {
	class Engine;

	class IBERUS_API Application {
	public:
		Application();
		virtual ~Application();

		virtual void Boot();
		virtual void Update();

		void Run();
	private:
		Engine* engine{ nullptr };
		std::unique_ptr<Window> window;

		bool running{true};
	};

	//To be defined in client
	Application* CreateApplication();
}


