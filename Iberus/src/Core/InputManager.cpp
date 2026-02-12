#include "Enginepch.h"
#include "InputManager.h"
#include "EventImport.h"
#include "Event.h"

namespace Iberus {

	void InputManager::OnEvent(Event& event) {
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e) {
			keyState[e.GetKeyCode()] = true;
			modifierState = e.GetModifiers();
			return false;
		});

		dispatcher.Dispatch<KeyReleasedEvent>([this](KeyReleasedEvent& e) {
			keyState[e.GetKeyCode()] = false;
			modifierState = e.GetModifiers();
			return false;
		});

		dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e) {
			mouseState[e.GetMouseButton()] = true;
			return false;
		});

		dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& e) {
			mouseState[e.GetMouseButton()] = false;
			return false;
		});

		dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& e) {
			mousePosition = e.GetMousePos();
			return false;
		});

		dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& e) {
			scrollDelta = scrollDelta + e.GetScrollOffset();
			return false;
		});
	}

	void InputManager::OnFrameEnd() {
		scrollDelta = Vec2(0, 0);
	}

	bool InputManager::IsKeyPressed(KeyCode key) const {
		auto it = keyState.find(key);
		return it != keyState.end() && it->second;
	}

	bool InputManager::IsMouseButtonPressed(MouseCode button) const {
		auto it = mouseState.find(button);
		return it != mouseState.end() && it->second;
	}

	bool InputManager::IsModifierPressed(ModifierFlags flag) const {
		return HasFlag(modifierState, flag);
	}

}
