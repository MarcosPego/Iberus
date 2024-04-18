#include "Enginepch.h"
#include "RenderPass.h"

#include "Framebuffer.h"

namespace Iberus {

	RenderPass::RenderPass(Framebuffer* inSourceFBO, Framebuffer* inTargetFBO) {
		sourceBuffer = inSourceFBO;
		targetBuffer = inTargetFBO;
	}

	void RenderPass::SetShader(ShaderApi* inShader) {
		shaderPass = inShader;
	}

}