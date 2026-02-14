#pragma once

#include "Core.h"

namespace Iberus {
	class World;
	class RenderBatch;
	class RenderCmd;

	class IBERUS_API LightSystem {
	public:
		static void Execute(World& world, RenderBatch& renderBatch);
	};

}
