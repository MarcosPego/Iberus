#pragma once

namespace Iberus {
	struct Frame;
	class ShaderApi;

	class RenderPass {
	public:
		virtual void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) = 0;
	};
}



