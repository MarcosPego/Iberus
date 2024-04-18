#pragma once

#include "ResourceManager.h"

namespace Iberus {
	class Mesh;

	class IBERUS_API MeshFactory {
	public:
		static Mesh* CreateQuad(const std::string& ID, ResourceManager& resourceManager, int width, int height);

		static Mesh* CreatePlane(const std::string& ID, ResourceManager& resourceManager, int width, int height, std::vector<float> heightMap);
	};
}



