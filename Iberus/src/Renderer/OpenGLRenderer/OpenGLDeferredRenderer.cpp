#include "Enginepch.h"
#include "OpenGLDeferredRenderer.h"


namespace Iberus {

	OpenGLDeferredRenderer::OpenGLDeferredRenderer() : OpenGLRenderer() {

	}

	void OpenGLDeferredRenderer::Init() {
		renderPasses.emplace_back(new OpenGLGeometryPass());
		renderPasses.emplace_back(new OpenGLRaymarchingPass());
		renderPasses.emplace_back(new OpenGLDeferredLightPass());
	}

	void OpenGLDeferredRenderer::RenderFrame(Frame& frame) {
		ExecuteAndFlushCmdQueue();

		auto _renderBatchCommands = [&](Frame& frame, ShaderApi* globalShader) {
			RenderBatchCommands(frame, globalShader);
		};

		for (const auto& pass : renderPasses) {
			pass->ExecutePass(frame, _renderBatchCommands);
		}
	}
}