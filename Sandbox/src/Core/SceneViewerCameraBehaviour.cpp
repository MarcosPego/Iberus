#include "SceneViewerCameraBehaviour.h"
#include "Engine.h"
#include "InputManager.h"
#include "KeyCode.h"
#include "MouseCode.h"
#include "Components.h"

using namespace Math;
using namespace Iberus;

std::unordered_map<SceneViewerAction, SceneViewerKeyBind> SceneViewerCameraBehaviour::CreateDefaultKeyBinds() {
	std::unordered_map<SceneViewerAction, SceneViewerKeyBind> binds;
	binds[SceneViewerAction::Toggle] = KeyCode::C;
	binds[SceneViewerAction::MoveForward] = KeyCode::W;
	binds[SceneViewerAction::MoveBackward] = KeyCode::S;
	binds[SceneViewerAction::MoveLeft] = KeyCode::A;
	binds[SceneViewerAction::MoveRight] = KeyCode::D;
	binds[SceneViewerAction::MoveUp] = KeyCode::E;
	binds[SceneViewerAction::MoveDown] = KeyCode::Q;
	binds[SceneViewerAction::Rotate] = MouseCode::Right;
	return binds;
}

bool SceneViewerCameraBehaviour::IsKeyBindPressed(const SceneViewerKeyBind& bind) const {
	auto& input = Engine::Instance()->GetInputManager();
	return std::visit([&input](auto&& arg) -> bool {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, KeyCode>) {
			return input.IsKeyPressed(arg);
		} else if constexpr (std::is_same_v<T, MouseCode>) {
			return input.IsMouseButtonPressed(arg);
		}
		return false;
	}, bind);
}

void SceneViewerCameraBehaviour::SetKeyBind(SceneViewerAction action, KeyCode key) {
	keyBinds[action] = key;
}

void SceneViewerCameraBehaviour::SetKeyBind(SceneViewerAction action, MouseCode button) {
	keyBinds[action] = button;
}

void SceneViewerCameraBehaviour::Init(EntityId entity, World& world) {
	enabled = false;
	wasTogglePressedLastFrame = false;
	lastMousePosition = Vec2(0, 0);
	isDragging = false;
	if (auto* transform = world.GetComponent<TransformComponent>(entity)) {
		pitch = transform->Rotation.x;
		yaw = transform->Rotation.y;
	}
}

void SceneViewerCameraBehaviour::Update(EntityId entity, World& world, double deltaTime) {
	auto& input = Engine::Instance()->GetInputManager();
	bool togglePressed = IsKeyBindPressed(keyBinds[SceneViewerAction::Toggle]);

	// Toggle on key press (edge detection)
	if (togglePressed && !wasTogglePressedLastFrame) {
		enabled = !enabled;
	}
	wasTogglePressedLastFrame = togglePressed;

	if (!enabled) {
		return;
	}

	// Mouse rotation
	if (IsKeyBindPressed(keyBinds[SceneViewerAction::Rotate])) {
		Vec2 currentMouse = input.GetMousePosition();

		if (!isDragging) {
			isDragging = true;
			lastMousePosition = currentMouse;
		} else {
			Vec2 delta = currentMouse - lastMousePosition;
			yaw -= delta.x * mouseSensitivity;  // Mouse right = rotate right
			pitch -= delta.y * mouseSensitivity; // Mouse up = look up (inverted from screen coords)

			// Clamp pitch to avoid flip
			const float maxPitch = 89.0f;
			if (pitch > maxPitch) pitch = maxPitch;
			if (pitch < -maxPitch) pitch = -maxPitch;

			if (auto* transform = world.GetComponent<TransformComponent>(entity)) {
				transform->Rotation = Vec3(pitch, yaw, 0.0f);
			}
			lastMousePosition = currentMouse;
		}
	} else {
		isDragging = false;
	}

	// WASD = forward/back, strafe. QE = up/down. (X=right, Y=forward, Z=up)
	float yawRad = Deg2Rad(yaw);
	float cosYaw = cosf(yawRad);
	float sinYaw = sinf(yawRad);

	Vec3 forwardXY(sinYaw, cosYaw, 0.0f);  // Forward in XY plane
	Vec3 rightXY(cosYaw, -sinYaw, 0.0f);    // Right in XY plane

	Vec3 movement(0, 0, 0);
	if (IsKeyBindPressed(keyBinds[SceneViewerAction::MoveForward])) movement += forwardXY;
	if (IsKeyBindPressed(keyBinds[SceneViewerAction::MoveBackward])) movement -= forwardXY;
	if (IsKeyBindPressed(keyBinds[SceneViewerAction::MoveLeft])) movement -= rightXY;
	if (IsKeyBindPressed(keyBinds[SceneViewerAction::MoveRight])) movement += rightXY;
	if (IsKeyBindPressed(keyBinds[SceneViewerAction::MoveUp])) movement.z += 1.0f;
	if (IsKeyBindPressed(keyBinds[SceneViewerAction::MoveDown])) movement.z -= 1.0f;

	if (movement.length() > 0) {
		movement = normalize(movement);
		if (auto* transform = world.GetComponent<TransformComponent>(entity)) {
			transform->Position = transform->Position + movement * moveSpeed * static_cast<float>(deltaTime);
		}
	}
}
