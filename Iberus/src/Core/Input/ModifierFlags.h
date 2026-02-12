#pragma once

namespace Iberus {

	// Modifier key flags (bitmask).
	enum ModifierFlags : int {
		NoModifiers = 0,
		Shift = 1 << 0,
		Ctrl = 1 << 1,
		Alt = 1 << 2,
		Super = 1 << 3,
		CapsLock = 1 << 4,
		NumLock = 1 << 5
	};

	inline ModifierFlags operator|(ModifierFlags a, ModifierFlags b) {
		return static_cast<ModifierFlags>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline ModifierFlags operator&(ModifierFlags a, ModifierFlags b) {
		return static_cast<ModifierFlags>(static_cast<int>(a) & static_cast<int>(b));
	}

	inline bool HasFlag(ModifierFlags flags, ModifierFlags flag) {
		return (static_cast<int>(flags) & static_cast<int>(flag)) != 0;
	}

}
