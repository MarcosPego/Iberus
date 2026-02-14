#include "Enginepch.h"
#include "World.h"

namespace Iberus {

	EntityId World::CreateEntity() {
		EntityId id = nextEntityId++;
		alive.insert(id);
		return id;
	}

	void World::DestroyEntity(EntityId entity) {
		alive.erase(entity);
		for (auto& [_, storage] : storages) {
			storage->Remove(entity);
		}
	}

	bool World::IsAlive(EntityId entity) const {
		return entity != NullEntity && alive.find(entity) != alive.end();
	}

}
