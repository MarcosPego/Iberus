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

		virtual void Bind(FramebufferMode mode, int drawTarget = 0) = 0;
		virtual void BindTextures(std::vector<int> targetIdx) = 0;
	};
}



