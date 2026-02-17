#pragma once

namespace Iberus {

	class Scene;

	class CustomSceneCreator {
	public:
		static void Create();

		/// Load shaders, meshes, textures and create materials on the scene. Call before loading or creating entities.
		static void SetupResourcesAndMaterials(class Scene* scene);
	};

}
