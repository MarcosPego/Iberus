#pragma once
#include <SceneInclude.h>

class SandboxBehaviour2 : public Iberus::Behaviour {
public:
	SandboxBehaviour2() { type = "b_sandbox2"; }

	void Init() override;

	void Update(double deltaTime) override;

};

