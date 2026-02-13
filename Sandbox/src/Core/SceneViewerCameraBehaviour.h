#pragma once

#include <SceneInclude.h>
#include <unordered_map>
#include <variant>
#include "KeyCode.h"
#include "MouseCode.h"

namespace Iberus {

	enum class SceneViewerAction {
		Toggle,
		MoveForward,
		MoveBackward,
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		Rotate
	};

	using SceneViewerKeyBind = std::variant<KeyCode, MouseCode>;

}

class SceneViewerCameraBehaviour : public Iberus::Behaviour {
public:
	SceneViewerCameraBehaviour() : keyBinds(CreateDefaultKeyBinds()) { type = "b_scene_viewer_camera"; }

	void Init() override;
	void Update(double deltaTime) override;

	void SetKeyBind(Iberus::SceneViewerAction action, Iberus::KeyCode key);
	void SetKeyBind(Iberus::SceneViewerAction action, Iberus::MouseCode button);

private:
	static std::unordered_map<Iberus::SceneViewerAction, Iberus::SceneViewerKeyBind> CreateDefaultKeyBinds();
	bool IsKeyBindPressed(const Iberus::SceneViewerKeyBind& bind) const;

	std::unordered_map<Iberus::SceneViewerAction, Iberus::SceneViewerKeyBind> keyBinds;
	bool enabled{ false };
	bool wasTogglePressedLastFrame{ false };
	Math::Vec2 lastMousePosition{ 0, 0 };
	bool isDragging{ false };
	float yaw{ 0.0f };   // degrees, rotation around world Y
	float pitch{ 0.0f }; // degrees, rotation around local X
	float moveSpeed{ 5.0f };
	float mouseSensitivity{ 0.15f };
};
