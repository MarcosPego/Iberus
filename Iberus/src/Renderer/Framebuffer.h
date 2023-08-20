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

		virtual void Bind(FramebufferMode mode, int drawTarget = 0, std::vector<int> bindIdxs = {}) = 0;
		//virtual void BindTextures(std::vector<int> targetIdx) = 0; See if I still need this

		virtual GLuint GetFBO() const = 0;
	};
}



