#pragma once

#include "Renderer.h"

namespace Iberus {
	class ShaderApi;

	class OpenGLRenderer : public Renderer {
	public:
		OpenGLRenderer();

		void Init() override {};

		void RenderFrame(Frame& frame) override;

		void ExecuteAndFlushCmdQueue() override;

		Framebuffer* CreateFramebuffer(const std::string& ID, const std::vector<TextureApi*>& inTextures) override;

	protected:
		virtual void RenderBatchCommands(Frame& frame, ShaderApi* globalShader = nullptr);

		std::unordered_map<int, int> openGLTexBindings;
	};
}



