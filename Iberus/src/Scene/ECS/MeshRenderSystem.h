#pragma once

#include "Core.h"
#include "World.h"

namespace Iberus {

	class Material;
	class Mesh;
	class RenderBatch;
	class Scene;

	class IBERUS_API MeshRenderSystem {
	public:
		/// Iterate MeshRenderer + Transform, push to RenderBatch. Needs Scene for materials, Engine for meshes.
		static void Execute(World& world, Scene& scene, RenderBatch& renderBatch);
	};

}
