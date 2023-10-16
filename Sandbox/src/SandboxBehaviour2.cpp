#include "SandboxBehaviour2.h"


void SandboxBehaviour2::Init() {


}

void SandboxBehaviour2::Update(double deltaTime) {
	static int count = 0;
	static Vec3 direction = Vec3(1, -1, 0);
	auto previousPos = root->GetPosition();

	root->SetPosition(previousPos + direction * 0.05f);

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
	direction = Vec3(moveX, moveY, 0);
	direction = normalize(direction);
	count++;
}