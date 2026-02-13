#include "SandboxBehaviour2.h"
#include "Engine.h"
#include "InputManager.h"
#include "KeyCode.h"

void SandboxBehaviour2::Init() {
	entity = dynamic_cast<Iberus::SDFEntity*>(root);
}

void SandboxBehaviour2::Update(double deltaTime) {
	if (!entity) {
		return;
	}

	auto& input = Iberus::Engine::Instance()->GetInputManager();
	Vec3 direction(0, 0, 0);
	if (input.IsKeyPressed(Iberus::KeyCode::W)) direction.y += 1;
	if (input.IsKeyPressed(Iberus::KeyCode::S)) direction.y -= 1;
	if (input.IsKeyPressed(Iberus::KeyCode::A)) direction.x += 1;
	if (input.IsKeyPressed(Iberus::KeyCode::D)) direction.x -= 1;
	if (input.IsKeyPressed(Iberus::KeyCode::Q)) direction.z += 1;
	if (input.IsKeyPressed(Iberus::KeyCode::E)) direction.z -= 1;

	if (direction.length() > 0) {
		root->SetPosition(root->GetPosition() + normalize(direction) * 0.05f);
	}

	Vec3 lastPos = root->GetPosition();
	for (auto& part : entity->GetParts()) {
		if ((lastPos - part.GetPosition()).length() > maxTension) {
			auto dir = lastPos - part.GetPosition();
			part.SetPosition(part.GetPosition() + normalize(dir) * 0.05f);
		}
		lastPos = part.GetPosition();
	}
}