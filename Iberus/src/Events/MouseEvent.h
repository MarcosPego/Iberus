#pragma once

#include "Event.h"

#include "Enginepch.h"
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
		inline int GetMouseButton() const { return button; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << button;
			return ss.str();
		}

		//EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseButtonEvent(int button) : button(button) {}
		int button;
	};

	class IBERUS_API MouseButtonPressedEvent final : public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class IBERUS_API MouseButtonReleasedEvent final : public MouseButtonEvent {
	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}