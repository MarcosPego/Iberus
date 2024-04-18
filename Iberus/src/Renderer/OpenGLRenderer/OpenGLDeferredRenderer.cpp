#include "Enginepch.h"
#include "OpenGLDeferredRenderer.h"

#include "TextureApi.h"
#include "Engine.h"
#include "Window.h"
#include "Framebuffer.h"

namespace Iberus {

	OpenGLDeferredRenderer::OpenGLDeferredRenderer() : OpenGLRenderer() {

	}

	void OpenGLDeferredRenderer::Init() {
		/// Setup both swap FBOs
		std::vector<std::string>  textures = { "worldPosOut_1", "diffuseOut_1", "normalOut_1", "uvsOut_1" };
		std::vector<TextureApi*> texturesAPI;
		for (const auto& entry : textures) {
			texturesAPI.push_back(dynamic_cast<TextureApi*>(GetResource(entry)));
		}

		sourceFBO = CreateFramebuffer("fbo_1", texturesAPI);

		textures = { "worldPosOut_2", "diffuseOut_2", "normalOut_2", "uvsOut_2" };
		texturesAPI.clear();
		for (const auto& entry : textures) {
			texturesAPI.push_back(dynamic_cast<TextureApi*>(GetResource(entry)));
		}
		targetFBO = CreateFramebuffer("fbo_2", texturesAPI);

		/// Emplace render passes
		renderPasses.emplace_back(new OpenGLGeometryPass(sourceFBO, targetFBO)); 		std::swap(sourceFBO, targetFBO);
		renderPasses.emplace_back(new OpenGLRaymarchingPass(sourceFBO, targetFBO));		std::swap(sourceFBO, targetFBO);
		renderPasses.emplace_back(new OpenGLDeferredLightPass(sourceFBO, targetFBO));	std::swap(sourceFBO, targetFBO);
		renderPasses.emplace_back(new OpenGLHDRPass(sourceFBO, targetFBO));				std::swap(sourceFBO, targetFBO);

		std::swap(sourceFBO, targetFBO);
	}

	void OpenGLDeferredRenderer::RenderFrame(Frame& frame) {
		ExecuteAndFlushCmdQueue();

		auto _renderBatchCommands = [&](Frame& frame, ShaderApi* globalShader) {
			RenderBatchCommands(frame, globalShader);
		};

		for (const auto& pass : renderPasses) {
			pass->ExecutePass(frame, _renderBatchCommands);
		}

		/// Blit the final target fbo to the default frame buffer
		auto* window = Engine::Instance()->GetCurrentWindow();
		auto width = window->GetWidth();
		auto height = window->GetWidth();
		glBlitNamedFramebuffer(targetFBO->GetFBO(), 0, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);	
	}
}