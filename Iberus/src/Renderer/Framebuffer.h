#pragma once

#include "RenderObject.h"

namespace Iberus {

	enum class FramebufferMode {
		WRITING,
		READING
	};

	class Framebuffer : public RenderObject {
	public:
		Framebuffer(const std::string ID);

		virtual void Bind(FramebufferMode mode) = 0;
	};
}



