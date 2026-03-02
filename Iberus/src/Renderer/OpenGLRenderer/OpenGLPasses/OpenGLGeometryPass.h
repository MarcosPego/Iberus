#pragma once

#include "RenderPass.h"

namespace Iberus {
	class ShaderApi;

	class OpenGLGeometryPass : public RenderPass {
	public:
		OpenGLGeometryPass();

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame, Framebuffer* source = nullptr, Framebuffer* target = nullptr) override;
		std::string GetName() const override { return "Geometry"; }
	};

}


