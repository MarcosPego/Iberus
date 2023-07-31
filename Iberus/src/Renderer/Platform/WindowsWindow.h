#pragma once

#include "Enginepch.h"
#include "Window.h"

namespace Iberus {
	class WindowsWindow : public Window {
	public:
		WindowsWindow(const Vec2& resolution, const std::string& title);
		explicit WindowsWindow(const WindowProps& newWindowProps);

		virtual ~WindowsWindow() override;

		void Update() override;

		virtual void Shutdown();

		void SetVSync(bool enabled) override;

	private: // Window internal setup

		virtual void SetupGraphicalContext();
		virtual void SetupWindow();


	private: // Properties
		GLFWwindow* window;

		struct WindowData {
			std::string title{ "Iberus" };
			Vec2 resolution{ 1330.0f, 720.0f };
			bool vsync{ true };
		};

		WindowData windowData;
	};
}
