#pragma once

#include "Enginepch.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {

	struct WindowProps {
		std::string title{ "Iberus" };
		Vec2 resolution{ 720.0f, 720.0f };
		bool vsync{ true };
		bool isFullScreen{ false };

		WindowProps() {}
		WindowProps(const Vec2& newResolution, const std::string& newTitle, bool fullscreen, bool newVsync = true) {
			resolution = newResolution;
			title = newTitle;
			vsync = newVsync;
			isFullScreen = fullscreen;
		}
	};

	class Window {
	public:
		Window(const Vec2& resolution, const std::string& title);
		explicit Window(const WindowProps& newWindowProps);

		virtual ~Window() {};

		virtual const std::string& GetTitle() const { return  windowProps.title; }

		virtual int GetWidth() const { return windowProps.resolution.x; }
		virtual int GetHeight() const { return  windowProps.resolution.y; }

		virtual void Update() = 0;

		virtual void SetVSync(bool enabled) { windowProps.vsync = enabled; }
		virtual bool IsVSync() const { return windowProps.vsync; }

		static Window* Create(const WindowProps& props = WindowProps());

	protected:
		WindowProps windowProps;
	};
}

