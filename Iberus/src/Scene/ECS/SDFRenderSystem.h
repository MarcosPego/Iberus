#pragma once

#include "Core.h"
#include "World.h"

namespace Iberus {

	class RenderBatch;
	class Scene;

	class IBERUS_API SDFRenderSystem {
	public:
		/// Renders all SDF entities, each with its own slot index.
		static void Execute(World& world, Scene& scene, RenderBatch& renderBatch);
	};

}
