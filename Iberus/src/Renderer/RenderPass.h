#pragma once

#include <string>

namespace Iberus {
	struct Frame;
	class Framebuffer;
	class ShaderApi;

	class RenderPass {
	public:
		RenderPass(Framebuffer* inSourceFBO = nullptr, Framebuffer* inTargetFBO = nullptr);
		virtual void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) = 0;
		virtual std::string GetName() const { return "Pass"; }

		void SetShader(ShaderApi* inShader);

	protected:
		Framebuffer* sourceBuffer{ nullptr };
		Framebuffer* targetBuffer{ nullptr };

		ShaderApi* shaderPass{ nullptr };
	};
}



