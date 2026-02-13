#pragma once

#include "Layer.h"

namespace Iberus {

	class IBERUS_API GameLayer : public Layer {
	public:
		GameLayer() : Layer("Game") {}

		void OnUpdate(double deltaTime) override;
	};

}
