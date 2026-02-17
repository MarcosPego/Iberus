#pragma once
#include <SceneInclude.h>

class SandboxBehaviour2 : public Iberus::Behaviour {
public:
	SandboxBehaviour2() { type = "b_sandbox2"; }

	void Init(Iberus::EntityId entity, Iberus::World& world) override;
	void Update(Iberus::EntityId entity, Iberus::World& world, double deltaTime) override;

private:
	float maxTension{ 2.0f };
};

