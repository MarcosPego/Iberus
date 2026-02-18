#pragma once

#include "ResourceManager.h"

namespace Iberus {
	class Mesh;

	class IBERUS_API MeshFactory {
	public:
		static Mesh* CreateCube(const std::string& ID, ResourceManager& resourceManager);
		static Mesh* CreateQuad(const std::string& ID, ResourceManager& resourceManager, int width, int height);
		static Mesh* CreateNDCQuad(const std::string& ID, ResourceManager& resourceManager);

		static Mesh* CreatePlane(const std::string& ID, ResourceManager& resourceManager, int width, int height,
			const std::vector<float>& heightMap, float heightScale = 1.0f, float worldSizeX = 60.0f, float worldSizeZ = 60.0f);
	};
}



