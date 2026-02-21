#pragma once

#include <string>

namespace Iberus {
	struct Frame;
	class Framebuffer;
	class ShaderApi;

	class RenderPass {
	public:
		RenderPass() = default;
		virtual void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame, Framebuffer* source = nullptr, Framebuffer* target = nullptr) = 0;
		virtual std::string GetName() const { return "Pass"; }

		void SetShader(ShaderApi* inShader);

		void SetEnabled(bool enabled) { enabledFlag = enabled; }
		bool IsEnabled() const { return enabledFlag; }

	protected:
		ShaderApi* shaderPass{ nullptr };
		bool enabledFlag{ true };
	};
}



