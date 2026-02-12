#pragma once

#include "OpenGLRenderer.h"

#include "RenderPass.h"

namespace Iberus {

	class OpenGLDeferredRenderer : public OpenGLRenderer {
	public:
		OpenGLDeferredRenderer();

		void Init() override;

		void RenderFrame(Frame& frame) override;

		void Resize(int width, int height) override;

	private:
		Framebuffer* sourceFBO{ nullptr };
		Framebuffer* targetFBO{ nullptr };
	};

}


