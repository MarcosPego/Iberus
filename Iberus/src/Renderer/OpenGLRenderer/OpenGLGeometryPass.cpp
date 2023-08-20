#include "Enginepch.h"
#include "OpenGLGeometryPass.h"

#include "Engine.h"

#include "Renderer.h"
#include "ShaderApi.h"
#include "TextureApi.h"
#include "Framebuffer.h"

#ifndef DEBUG_FBO
#define DEBUG_FBO 0
#endif

#ifdef DEBUG_FBO
#include "OpenGLFrameBuffer.h"
#include "Window.h"
#endif

namespace Iberus {

	OpenGLGeometryPass::OpenGLGeometryPass() {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		auto* shaderObject = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseGeometryShader"));
		if (shaderObject) {
			shaderPass = shaderObject;
		}
		
		textures = { "worldPosOut_1", "diffuseOut_1", "normalOut_1", "uvsOut_1" };
		std::vector<TextureApi*> texturesAPI;
		for (const auto& entry : textures) {
			texturesAPI.push_back(dynamic_cast<TextureApi*>(renderer.GetResource(entry)));
		}

		frameBuffer = renderer.CreateFramebuffer("geometryFBO", texturesAPI);
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

	
#ifdef DEBUG_FBO
		// Copy fbo to the default fbo (draw to screen)
		auto fboID = dynamic_cast<OpenGLFramebuffer*>(frameBuffer)->GetFBO();
		auto* currentWindow = Engine::Instance()->GetCurrentWindow();
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, currentWindow->GetWidth(), currentWindow->GetHeight(),
			0, 0, currentWindow->GetWidth(), currentWindow->GetHeight(),
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
#endif
	}

}