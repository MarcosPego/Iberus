#include "Enginepch.h"
#include "OpenGLGeometryPass.h"

#include "Engine.h"

#include "Renderer.h"
#include "ShaderApi.h"
#include "Framebuffer.h"

namespace Iberus {

	OpenGLGeometryPass::OpenGLGeometryPass() {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		auto* shaderObject = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseGeometryShader"));
		if (shaderObject) {
			shaderPass = shaderObject;
		}
	}

	void OpenGLGeometryPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) {
		// Only the geometry pass updates the depth buffer
		shaderPass->Bind();
		frameBuffer->Bind(FramebufferMode::WRITING);

		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		renderFrame(frame, shaderPass);

		// When we get here the depth buffer is already populated and the stencil pass
		// depends on it, but it does not write to it.
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}

}