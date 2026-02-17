#pragma once

#include "SceneInclude.h"
#include "KeyCode.h"
#include "MouseCode.h"

#include <unordered_map>
#include <variant>

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

	class IBERUS_API SceneViewerCameraBehaviour : public Behaviour {
	public:
		SceneViewerCameraBehaviour();

		void Init(EntityId entity, World& world) override;
		void Update(EntityId entity, World& world, double deltaTime) override;

		void SetKeyBind(SceneViewerAction action, KeyCode key);
		void SetKeyBind(SceneViewerAction action, MouseCode button);

	private:
		static std::unordered_map<SceneViewerAction, SceneViewerKeyBind> CreateDefaultKeyBinds();
		bool IsKeyBindPressed(const SceneViewerKeyBind& bind) const;

		std::unordered_map<SceneViewerAction, SceneViewerKeyBind> keyBinds;
		bool enabled{ false };
		bool wasTogglePressedLastFrame{ false };
		Math::Vec2 lastMousePosition{ 0, 0 };
		bool isDragging{ false };
		float yaw{ 0.0f };
		float pitch{ 0.0f };
		float moveSpeed{ 5.0f };
		float mouseSensitivity{ 0.15f };
	};

}
