#pragma once

#include "Event.h"
#include "Core/Input/MouseCode.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	class IBERUS_API MouseMovedEvent final : public Event {
	public:
		MouseMovedEvent(const Vec2& mousePos) : mousePosition(mousePos) {}

		inline Vec2 GetMousePos() const {
			return mousePosition;
		}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseMovedEvent: " << mousePosition.x << ", " << mousePosition.y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		Vec2 mousePosition;
	};

	class IBERUS_API MouseScrolledEvent final : public Event {
	public:
		MouseScrolledEvent(const Vec2& scrollOffset) : scrollOffset(scrollOffset) {}

		inline Vec2 GetScrollOffset() const { return scrollOffset; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << scrollOffset.x << ", " << scrollOffset.y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		Vec2 scrollOffset;
	};

	class IBERUS_API MouseButtonEvent : public Event {
	public:
		inline MouseCode GetMouseButton() const { return button; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonEvent: " << static_cast<int>(button);
			return ss.str();
		}

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseButtonEvent(MouseCode btn) : button(btn) {}
		MouseCode button;
	};

	class IBERUS_API MouseButtonPressedEvent final : public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(MouseCode btn) : MouseButtonEvent(btn) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << static_cast<int>(button);
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class IBERUS_API MouseButtonReleasedEvent final : public MouseButtonEvent {
	public:
		MouseButtonReleasedEvent(MouseCode btn) : MouseButtonEvent(btn) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << static_cast<int>(button);
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}