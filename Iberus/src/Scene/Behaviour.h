#pragma once

#include "EntityId.h"

namespace Iberus {
	class World;

	class IBERUS_API Behaviour {
	public:
		virtual ~Behaviour() = default;

		virtual void Init(EntityId entity, World& world) {}
		virtual void Update(EntityId entity, World& world, double deltaTime) {}

		void BindBehaviour(EntityId entity, World& world);

		const std::string& GetID() const { return ID; }
		const std::string& GetType() const { return type; }

	protected:
		std::string type{ "b_behaviour" };
		std::string ID;
	};
}

