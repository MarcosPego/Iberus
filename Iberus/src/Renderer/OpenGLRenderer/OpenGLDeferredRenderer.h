#pragma once

#include "OpenGLRenderer.h"

#include "RenderPass.h"

namespace Iberus {

	class OpenGLDeferredRenderer : public OpenGLRenderer {
	public:
		OpenGLDeferredRenderer();

		void Init() override;

		void RenderFrame(Frame& frame) override;

	private:
	
	};

}


