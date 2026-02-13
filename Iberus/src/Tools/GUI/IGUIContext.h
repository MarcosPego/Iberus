#pragma once

#include "Core.h"

namespace Iberus {

	class IBERUS_API IGUIContext {
	public:
		virtual ~IGUIContext() = default;

		virtual bool BeginWindow(const char* title) = 0;
		virtual void EndWindow() = 0;
		virtual void Text(const char* text) = 0;
		virtual bool Button(const char* label) = 0;

		virtual void BeginDockSpace() = 0;
		virtual void EndDockSpace() = 0;
		virtual void Image(void* textureId, float w, float h) = 0;
	};

}
