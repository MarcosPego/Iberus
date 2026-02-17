#include "SandboxBehaviour.h"
#include "Components.h"

using namespace Math;

void SandboxBehaviour::Init(Iberus::EntityId entity, Iberus::World& world) {
	(void)entity;
	(void)world;
}

void SandboxBehaviour::Update(Iberus::EntityId entity, Iberus::World& world, double deltaTime) {
	static int count = 0;
	static Vec3 direction = Vec3(0, 1, 0);
	auto* transform = world.GetComponent<Iberus::TransformComponent>(entity);
	if (!transform) {
		return;
	}
	auto previousPos = transform->Position;
	transform->Position = previousPos + direction * 0.05f;

	//Iberus::Log::GetClientLogger()->info(std::format("Update: current surection Vec: {}, {}, {}", direction.x, direction.y, direction.z));
	
	static auto moveX = 0;
	static auto moveY = -1;

	if (count > 200) {
		if (moveX == 1) {
			moveX = 0;
			moveY = 1;
		}
		else if (moveY == 1) {
			moveX = -1;
			moveY = 0;
		}

		else if (moveX == -1) {
			moveX = 0;
			moveY = -1;
		}
		else if (moveY == -1) {
			moveX = 1;
			moveY = 0;
		}

		count = 0;
	}

	//auto r1 = ((double)rand() / (RAND_MAX)) + 0.5f;
	//auto r2 = ((double)rand() / (RAND_MAX)) + 0.5f;
	//auto r3 = ((double)rand() / (RAND_MAX)) + 0.5f;
	direction = Vec3(static_cast<float>(moveX), static_cast<float>(moveY), 0);
	direction = normalize(direction);
	count++;
	(void)deltaTime;
}