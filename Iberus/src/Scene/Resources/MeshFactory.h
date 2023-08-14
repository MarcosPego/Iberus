#pragma once

#include "ResourceManager.h"

namespace Iberus {
	class Mesh;

	class MeshFactory {
	public:
		static Mesh* CreateQuad(const std::string& ID, ResourceManager& resourceManager, int width, int height);
	};
}



