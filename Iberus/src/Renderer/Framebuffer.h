#pragma once

namespace Iberus {

	enum class FramebufferMode {
		WRITING,
		READING
	};

	class Framebuffer {
	public:
		virtual void Bind(FramebufferMode mode) = 0;
		virtual void Unbind() = 0;
	};
}



