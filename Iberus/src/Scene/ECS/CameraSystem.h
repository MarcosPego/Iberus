#pragma once

#include "Core.h"
#include "World.h"

namespace Iberus {

	class RenderBatch;

	class IBERUS_API CameraSystem {
	public:
		/// Push CameraRenderCmd for the active camera. Returns EntityId of active camera or NullEntity.
		static EntityId Execute(World& world, RenderBatch& renderBatch);
	};

}
