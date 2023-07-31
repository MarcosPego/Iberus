#include "Enginepch.h"

#include "Window.h"
#include "WindowsWindow.h"

namespace Iberus {

	Window* Window::Create(const WindowProps& props) {
		return new WindowsWindow(props);
	}

	Window::Window(const Vec2& resolution, const std::string& title) {
		windowProps = { resolution, title, true, true };
	}

	Window::Window(const WindowProps& newWindowProps) {
		windowProps = newWindowProps;
	}
}