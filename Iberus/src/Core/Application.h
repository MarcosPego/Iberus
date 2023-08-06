#pragma once

#include "Core.h"
#include "Window.h"
#include "Renderer.h"

#include "Scene.h" // temp

namespace Iberus {
	class IBERUS_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		std::unique_ptr<Window> window;
		std::unique_ptr<Renderer> renderer;

		// temp
		Scene* baseScene{ nullptr };
	
		bool running{true};
	};

	//To be defined in client
	Application* CreateApplication();
}


