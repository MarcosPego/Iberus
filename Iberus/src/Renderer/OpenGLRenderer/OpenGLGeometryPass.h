#pragma once

#include "RenderPass.h"

namespace Iberus {
	class ShaderApi;
	class Framebuffer;

	class OpenGLGeometryPass : public RenderPass {
	public:
		OpenGLGeometryPass();

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) override;
	private:
		Framebuffer* frameBuffer{ nullptr };
		ShaderApi* shaderPass{ nullptr };
		std::unordered_map<std::string, std::string> textures;
	};

}


