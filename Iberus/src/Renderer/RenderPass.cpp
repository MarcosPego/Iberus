#include "Enginepch.h"
#include "RenderPass.h"

#include "Framebuffer.h"

namespace Iberus {

	void RenderPass::SetShader(ShaderApi* inShader) {
		shaderPass = inShader;
	}

}