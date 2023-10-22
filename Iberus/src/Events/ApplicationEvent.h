#pragma once

#include "Enginepch.h"
#include "Event.h"

namespace Iberus {
	class ApplicationEvent
	{
	};

	class IBERUS_API WindowResizeEvent final : public Event {
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : height(height), width(width) {};

		inline 	uint32_t GetWidth() const { return width; }
		inline 	uint32_t GetHeight() const { return width; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "WindowResizeEvent: " << width << ", " << height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		uint32_t width, height;
	};

	class IBERUS_API WindowCloseEvent final : public Event{
		public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class IBERUS_API AppTickEvent final : public Event{
		public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class IBERUS_API AppUpdateEvent final : public Event{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class IBERUS_API AppRenderEvent final : public Event{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}