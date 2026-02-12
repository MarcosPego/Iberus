#pragma once

#include "Event.h"
#include "Core/Input/KeyCode.h"
#include "Core/Input/ModifierFlags.h"
#include <sstream>

namespace Iberus {

	class IBERUS_API KeyEvent : public Event {
	public:
		inline KeyCode GetKeyCode() const { return keyCode; }
		inline ModifierFlags GetModifiers() const { return modifiers; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(KeyCode code, ModifierFlags mods) : keyCode(code), modifiers(mods) {}

		KeyCode keyCode;
		ModifierFlags modifiers;
	};

	class IBERUS_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(KeyCode code, int repeatCount, ModifierFlags mods = ModifierFlags::NoModifiers)
			: KeyEvent(code, mods), repeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return repeatCount; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent: " << static_cast<int>(keyCode) << " (" << repeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int repeatCount;
	};

	class IBERUS_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(KeyCode code, ModifierFlags mods = ModifierFlags::NoModifiers)
			: KeyEvent(code, mods) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << static_cast<int>(keyCode);
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

}
