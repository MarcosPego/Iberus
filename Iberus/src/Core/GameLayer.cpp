#include "Enginepch.h"
#include "GameLayer.h"
#include "Engine.h"

namespace Iberus {

	void GameLayer::OnUpdate(double deltaTime) {
		Engine::Instance()->Update();
	}

}
