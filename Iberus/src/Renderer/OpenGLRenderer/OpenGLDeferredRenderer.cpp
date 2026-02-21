#include "Enginepch.h"
#include "OpenGLDeferredRenderer.h"

#include "TextureApi.h"
#include "Engine.h"
#include "Window.h"
#include "Framebuffer.h"
#include "Profiler.h"

#include <chrono>

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

	void OpenGLDeferredRenderer::RenderFrame(Frame& frame, unsigned int outputFBO, int outputWidth, int outputHeight) {
		ExecuteAndFlushCmdQueue();

		auto* window = Engine::Instance()->GetCurrentWindow();
		int width = outputWidth > 0 ? outputWidth : window->GetWidth();
		int height = outputHeight > 0 ? outputHeight : window->GetHeight();

		if (width != currentWidth || height != currentHeight) {
			Resize(width, height);
			currentWidth = width;
			currentHeight = height;
		}

		glViewport(0, 0, width, height);

		auto _renderBatchCommands = [&](Frame& frame, ShaderApi* globalShader) {
			RenderBatchCommands(frame, globalShader);
		};

		for (const auto& pass : renderPasses) {
			auto t0 = std::chrono::high_resolution_clock::now();
			pass->ExecutePass(frame, _renderBatchCommands);
			auto t1 = std::chrono::high_resolution_clock::now();
			double passMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
			Profiler::Instance().RecordZone(pass->GetName(), passMs);
		}

		unsigned int blitTarget = outputFBO != 0 ? outputFBO : 0;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, targetFBO->GetFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blitTarget);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLDeferredRenderer::Resize(int width, int height) {
		currentWidth = width;
		currentHeight = height;
		glViewport(0, 0, width, height);
		if (sourceFBO) {
			sourceFBO->ResizeAttachments(width, height);
		}
		if (targetFBO) {
			targetFBO->ResizeAttachments(width, height);
		}
	}
}