#include "Enginepch.h"
#include "Behaviour.h"
#include "World.h"
#include "Components.h"

namespace Iberus {

	void Behaviour::BindBehaviour(EntityId entity, World& world) {
		if (auto* tag = world.GetComponent<TagComponent>(entity)) {
			ID = "Behaviour_" + tag->Id;
		} else {
			ID = "Behaviour_" + std::to_string(entity);
		}
	}
}