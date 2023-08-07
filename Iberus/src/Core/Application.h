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

		void Boot();
		void Run();
	private:
		Engine* engine{ nullptr };
		std::unique_ptr<Window> window;
		// temp
		Scene* baseScene{ nullptr };
	
		bool running{true};
	};

	//To be defined in client
	Application* CreateApplication();
}


