#include "SandboxBehaviour2.h"

void SandboxBehaviour2::Init() {
	entity = dynamic_cast<Iberus::SDFEntity*>(root);
}

void SandboxBehaviour2::Update(double deltaTime) {
	if (!entity) {
		return;
	}

	static int count = 0;
	static Vec3 direction = Vec3(1, -1, 0);
	auto previousPos = root->GetPosition();


	root->SetPosition(previousPos + direction * 0.05f);
	//Iberus::Log::GetClientLogger()->info(std::format("Update: current surection Vec: {}, {}, {}", direction.x, direction.y, direction.z));

	static auto moveX = 0;
	static auto moveY = -1;

	if (count > 250) {
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
	direction = Vec3(moveX, moveY, 0);
	direction = normalize(direction);
	count++;

	Vec3 lastPos = root->GetPosition();
	for (auto& part : entity->GetParts()) {
		if ((lastPos - part.GetPosition()).length() > maxTension) {
			auto dir = lastPos - part.GetPosition();
			part.SetPosition(part.GetPosition() + normalize(dir) * 0.05f);
		}
		lastPos = part.GetPosition();
	}
}