#include "SandboxBehaviour.h"

using namespace Math;


void SandboxBehaviour::Init() {


}

void SandboxBehaviour::Update() {
	static Vec3 direction = Vec3(0, 1, 0);
	auto previousPos = root->GetPosition();

	root->SetPosition(previousPos + direction * 0.05f);

	//Iberus::Log::GetClientLogger()->info(std::format("Update: current surection Vec: {}, {}, {}", direction.x, direction.y, direction.z));

	auto r1 = ((double)rand() / (RAND_MAX)) + 0.5f;
	auto r2 = ((double)rand() / (RAND_MAX)) + 0.5f;
	auto r3 = ((double)rand() / (RAND_MAX)) + 0.5f;
	direction = Vec3(r1, r2, 0);
	direction = normalize(direction);
}