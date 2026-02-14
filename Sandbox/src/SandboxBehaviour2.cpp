#include "SandboxBehaviour2.h"
#include "Engine.h"
#include "InputManager.h"
#include "KeyCode.h"
#include "Components.h"

void SandboxBehaviour2::Init(Iberus::EntityId entity, Iberus::World& world) {
	(void)entity;
	(void)world;
}

void SandboxBehaviour2::Update(Iberus::EntityId entity, Iberus::World& world, double deltaTime) {
	auto* transform = world.GetComponent<Iberus::TransformComponent>(entity);
	auto* sdf = world.GetComponent<Iberus::SDFComponent>(entity);
	if (!transform || !sdf) {
		return;
	}

	auto& input = Iberus::Engine::Instance()->GetInputManager();
	Math::Vec3 direction(0, 0, 0);
	if (input.IsKeyPressed(Iberus::KeyCode::W)) { direction.y += 1; }
	if (input.IsKeyPressed(Iberus::KeyCode::S)) { direction.y -= 1; }
	if (input.IsKeyPressed(Iberus::KeyCode::A)) { direction.x += 1; }
	if (input.IsKeyPressed(Iberus::KeyCode::D)) { direction.x -= 1; }
	if (input.IsKeyPressed(Iberus::KeyCode::Q)) { direction.z += 1; }
	if (input.IsKeyPressed(Iberus::KeyCode::E)) { direction.z -= 1; }

	if (direction.length() > 0) {
		transform->Position = transform->Position + normalize(direction) * 0.05f;
	}

	Math::Vec3 lastPos = transform->Position;
	for (auto& part : sdf->Parts) {
		Math::Vec3 partWorldPos = transform->Position + part.Transform.Position;
		if ((lastPos - partWorldPos).length() > maxTension) {
			auto dir = lastPos - partWorldPos;
			part.Transform.Position = part.Transform.Position + normalize(dir) * 0.05f;
		}
		lastPos = transform->Position + part.Transform.Position;
	}
}