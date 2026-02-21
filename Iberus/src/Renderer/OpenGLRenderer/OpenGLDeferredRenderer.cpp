#include "Enginepch.h"
#include "OpenGLDeferredRenderer.h"

#include "TextureApi.h"
#include "Engine.h"
#include "Window.h"
#include "Framebuffer.h"
#include "Profiler.h"
#include "RenderSettings.h"
#include "OpenGLDeferredLightPass.h"
#include "OpenGLShader.h"
#include "ShaderApi.h"
#include "ShaderBindings.h"

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

		/// Emplace render passes (source/target passed at execute time; swap only after executed passes).
		/// Chain: Geometry -> Raymarch -> DeferredLight -> Outline -> HDR -> Pixelation -> blit.
		/// Uses same sourceFBO/targetFBO ping-pong; swap after each pass.
		/// Outline preserves worldPos/normal/uvs for HDR (HDR only needs color); future passes (fog, SSAO) insert as needed.
		renderPasses.emplace_back(new OpenGLGeometryPass());
		renderPasses.emplace_back(new OpenGLRaymarchingPass());
		renderPasses.emplace_back(new OpenGLDeferredLightPass());
		renderPasses.emplace_back(new OpenGLOutlinePass());
		renderPasses.emplace_back(new OpenGLHDRPass());
		renderPasses.emplace_back(new OpenGLPixelationPass());

		SyncLightPassShader();
	}

	void OpenGLDeferredRenderer::SyncLightPassShader() {
		const PostProcessSettings& settings = Engine::Instance()->GetPostProcessSettings();
		bool usePixelPipeline = (settings.renderStyle == RenderStyle::PixelStyle) || settings.enableToon;

		const char* shaderId = usePixelPipeline ? "assets/shaders/baseToonLightShader" : "assets/shaders/baseDeferredLightShader";
		ShaderApi* lightShader = dynamic_cast<ShaderApi*>(GetResource(shaderId));
		if (!lightShader) {
			lightShader = dynamic_cast<ShaderApi*>(GetResource("assets/shaders/baseDeferredLightShader"));
		}

		for (const auto& p : renderPasses) {
			RenderPass* pass = p.get();
			if (pass->GetName() == "Light") {
				pass->SetShader(lightShader);
				break;
			}
		}
	}

	void OpenGLDeferredRenderer::SetLightPassPipelineUniforms(RenderPass* lightPass) {
		const PostProcessSettings& settings = Engine::Instance()->GetPostProcessSettings();
		bool usePixelPipeline = (settings.renderStyle == RenderStyle::PixelStyle) || settings.enableToon;
		if (!usePixelPipeline) {
			return;
		}
		ShaderApi* shader = lightPass->GetShader();
		if (!shader) {
			return;
		}
		GLuint programID = 0;
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shader); openGLShader) {
			programID = openGLShader->GetProgramID();
		}
		if (programID == 0) {
			return;
		}
		const PostProcessSettings& pp = Engine::Instance()->GetPostProcessSettings();
		shader->Bind();
		ShaderBindings::SetUniform<int>(programID, "toonCuts", pp.toonCuts);
		ShaderBindings::SetUniform<float>(programID, "toonSteepness", pp.toonSteepness);
		ShaderBindings::SetUniform<float>(programID, "toonWrap", pp.toonWrap);
		ShaderBindings::SetUniform<float>(programID, "toonRimWidth", pp.toonRimWidth);
	}

	void OpenGLDeferredRenderer::RenderFrame(Frame& frame, unsigned int outputFBO, int outputWidth, int outputHeight) {
		ExecuteAndFlushCmdQueue();
		SyncLightPassShader();

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
			if (!pass->IsEnabled()) {
				continue;
			}
			if (pass->GetName() == "Light") {
				SetLightPassPipelineUniforms(pass.get());
			}
			auto t0 = std::chrono::high_resolution_clock::now();
			pass->ExecutePass(frame, _renderBatchCommands, sourceFBO, targetFBO);
			auto t1 = std::chrono::high_resolution_clock::now();
			double passMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
			Profiler::Instance().RecordZone(pass->GetName(), passMs);
			std::swap(sourceFBO, targetFBO);
		}

		unsigned int blitTarget = outputFBO != 0 ? outputFBO : 0;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, sourceFBO->GetFBO());
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