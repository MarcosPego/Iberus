#include "Enginepch.h"
#include "OpenGLForwardPass.h"

#include "ShaderApi.h"

#include "Engine.h"

namespace Iberus {

	OpenGLForwardPass::OpenGLForwardPass() {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		auto* shaderObject = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseShader"));
		if (shaderObject) {
			shaderPass = shaderObject;
		}
	}

	void OpenGLForwardPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) {
		if (!shaderPass) {
			return;
		}

		shaderPass->Bind();

		renderFrame(frame, shaderPass);
	}

}