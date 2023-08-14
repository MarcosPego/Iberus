#pragma once

#include "OpenGLRenderer.h"

#include "OpenGLGeometryPass.h"
#include "RenderPass.h"

namespace Iberus {

	class OpenGLDeferredRenderer : public OpenGLRenderer {
	public:
		OpenGLDeferredRenderer();

		void Init() override;

		void RenderFrame(Frame& frame) override;

	private:
		std::vector<std::unique_ptr<RenderPass>> renderPasses;
	};

}


