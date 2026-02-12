#pragma once

#include "Core/Input/KeyCode.h"
#include "Core/Input/MouseCode.h"
#include "Core/Input/ModifierFlags.h"

namespace Iberus {

	// Translates GLFW input codes to engine abstract types.
	// Only GLFW-specific translation code includes glfw3.h (in .cpp).
	class InputTranslationGLFW {
	public:
		static KeyCode FromGLFWKey(int glfwKey);
		static MouseCode FromGLFWMouseButton(int glfwButton);
		static ModifierFlags FromGLFWMods(int glfwMods);
	};

}
