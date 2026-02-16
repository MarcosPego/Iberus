#include "Enginepch.h"
#include "BehaviourSystem.h"
#include "Scene.h"
#include "Behaviour.h"
#include "Components.h"

namespace Iberus {

	void BehaviourSystem::Update(World& world, Scene& scene, double deltaTime) {
		for (auto& [type, list] : scene.GetRegisteredBehaviours()) {
			for (auto& [entityId, behaviour] : list) {
				if (!world.IsAlive(entityId)) {
					continue;
				}
				auto* active = world.GetComponent<ActiveComponent>(entityId);
				if (active && !active->Active) {
					continue;
				}
				behaviour->Update(entityId, world, deltaTime);
			}
		}
	}

}
