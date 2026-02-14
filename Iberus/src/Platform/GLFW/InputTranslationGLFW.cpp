#include "Enginepch.h"
#include "Platform/GLFW/InputTranslationGLFW.h"
#include <glfw3.h>

namespace Iberus {

	KeyCode InputTranslationGLFW::FromGLFWKey(int glfwKey) {
		if (glfwKey == GLFW_KEY_UNKNOWN) {
			return KeyCode::Unknown;
		}
		if (glfwKey >= 0 && glfwKey <= static_cast<int>(KeyCode::Menu)) {
			return static_cast<KeyCode>(glfwKey);
		}
		return KeyCode::Unknown;
	}

	MouseCode InputTranslationGLFW::FromGLFWMouseButton(int glfwButton) {
		switch (glfwButton) {
			case GLFW_MOUSE_BUTTON_LEFT:   return MouseCode::Left;
			case GLFW_MOUSE_BUTTON_RIGHT:  return MouseCode::Right;
			case GLFW_MOUSE_BUTTON_MIDDLE: return MouseCode::Middle;
			case 3: return MouseCode::Button4;
			case 4: return MouseCode::Button5;
			default: return MouseCode::Unknown;
		}
	}

	ModifierFlags InputTranslationGLFW::FromGLFWMods(int glfwMods) {
		ModifierFlags flags = ModifierFlags::NoModifiers;
		if (glfwMods & GLFW_MOD_SHIFT)    flags = flags | ModifierFlags::Shift;
		if (glfwMods & GLFW_MOD_CONTROL)  flags = flags | ModifierFlags::Ctrl;
		if (glfwMods & GLFW_MOD_ALT)      flags = flags | ModifierFlags::Alt;
		if (glfwMods & GLFW_MOD_SUPER)   flags = flags | ModifierFlags::Super;
		if (glfwMods & GLFW_MOD_CAPS_LOCK) flags = flags | ModifierFlags::CapsLock;
		if (glfwMods & GLFW_MOD_NUM_LOCK)  flags = flags | ModifierFlags::NumLock;
		return flags;
	}

}
