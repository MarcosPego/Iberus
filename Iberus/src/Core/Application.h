#pragma once

#include "Core.h"

namespace Iberus {
	class IBERUS_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	//To be defined in client
	Application* CreateApplication();
}


