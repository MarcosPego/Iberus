#include "Enginepch.h"
#include "OpenGLDeferredRenderer.h"


namespace Iberus {

	OpenGLDeferredRenderer::OpenGLDeferredRenderer() : OpenGLRenderer() {

	}

	void OpenGLDeferredRenderer::Init() {

	}

	void OpenGLDeferredRenderer::RenderFrame(Frame& frame) {
		ExecuteAndFlushCmdQueue();
		//RenderBatchCommands(frame);

		auto _renderBatchCommands = [&](Frame& frame, ShaderApi* globalShader) {
			RenderBatchCommands(frame, globalShader);
		};

		for (const auto& pass : renderPasses) {
			pass->ExecutePass(frame, _renderBatchCommands);
		}
	}
}