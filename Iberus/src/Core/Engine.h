#pragma once

#include "Renderer.h"

namespace Iberus {
	class Engine {
	public:
		static Engine* Instance();

		void Boot();

		void Update();

		Renderer& GetRenderer() { return *renderer.get(); }

	private:
		std::unique_ptr<Renderer> renderer;
	};
}



