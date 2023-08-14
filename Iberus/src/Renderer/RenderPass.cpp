#include "Enginepch.h"
#include "RenderPass.h"

namespace Iberus {

	void RenderPass::SetShader(ShaderApi* inShader) {
		shaderPass = inShader;
	}

}