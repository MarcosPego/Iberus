#pragma once

#include "Renderer.h"
#include "ResourceManager.h"

namespace Iberus {
	class Engine {
	public:
		static Engine* Instance();

		void Boot();

		void Update();

		Renderer& GetRenderer() { return *renderer.get(); }

		ResourceManager& GetResourceManager() { return *resourceManager.get(); }

	private:
		std::unique_ptr<Renderer> renderer;
		std::unique_ptr<ResourceManager> resourceManager;
	};
}



