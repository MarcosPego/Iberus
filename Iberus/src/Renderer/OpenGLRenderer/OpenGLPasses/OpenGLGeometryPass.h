#pragma once

#include "RenderPass.h"

namespace Iberus {
	class ShaderApi;

	class OpenGLGeometryPass : public RenderPass {
	public:
		OpenGLGeometryPass(Framebuffer* inSourceFBO = nullptr, Framebuffer* inTargetFBO = nullptr);

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) override;
	};

}


