#pragma once

#include "OpenGLRenderer.h"

#include "RenderPass.h"

namespace Iberus {

	class OpenGLDeferredRenderer : public OpenGLRenderer {
	public:
		OpenGLDeferredRenderer();

		void Init() override;

		void RenderFrame(Frame& frame, unsigned int outputFBO = 0, int outputWidth = 0, int outputHeight = 0) override;

		void Resize(int width, int height) override;

	private:
		void SyncLightPassShader();
		void SetLightPassPipelineUniforms(RenderPass* lightPass);

		Framebuffer* sourceFBO{ nullptr };
		Framebuffer* targetFBO{ nullptr };
		int currentWidth{ 0 };
		int currentHeight{ 0 };
	};

}


