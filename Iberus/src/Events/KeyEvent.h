#pragma once

#include "Event.h"
#include <sstream>


namespace Iberus {
	class IBERUS_API KeyEvent :  public Event {
	public: 
		inline int GetKeyCode() const { return currentKeyCode; }
	
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard, EventCategoryInput)
	protected:
		KeyEvent(int keycode) : currentKeyCode(keycode) {

		}

		int currentKeyCode;
	};

	class IBERUS_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), currentRepeatCount(repeatCount) {}

		std::string ToString() {
			std::stringstream ss;
			ss << "KeyPressedEvent: " << currentKeyCode << " (" << currentRepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
			int currentRepeatCount;
	};

	class IBERUS_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

		std::string ToString() {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << currentKeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}