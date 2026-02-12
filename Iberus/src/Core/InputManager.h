#pragma once

#include "Core.h"
#include "Core/Input/KeyCode.h"
#include "Core/Input/MouseCode.h"
#include "Core/Input/ModifierFlags.h"
#include "MathUtils.h"
#include <unordered_map>

using namespace Math;

namespace Iberus {

	class Event;

	class IBERUS_API InputManager {
	public:
		InputManager() = default;

		void OnEvent(Event& event);
		void OnFrameEnd();

		bool IsKeyPressed(KeyCode key) const;
		bool IsKeyDown(KeyCode key) const { return IsKeyPressed(key); }
		bool IsMouseButtonPressed(MouseCode button) const;
		bool IsModifierPressed(ModifierFlags flag) const;

		Vec2 GetMousePosition() const { return mousePosition; }
		Vec2 GetScrollDelta() const { return scrollDelta; }

	private:
		std::unordered_map<KeyCode, bool> keyState;
		std::unordered_map<MouseCode, bool> mouseState;
		ModifierFlags modifierState = ModifierFlags::NoModifiers;
		Vec2 mousePosition = Vec2(0, 0);
		Vec2 scrollDelta = Vec2(0, 0);
	};

}
