#pragma once

#include <SceneInclude.h>

class SandboxBehaviour : public Iberus::Behaviour {
public:
	SandboxBehaviour() { type = "b_sandbox"; }

	void Init(Iberus::EntityId entity, Iberus::World& world) override;
	void Update(Iberus::EntityId entity, Iberus::World& world, double deltaTime) override;

};

