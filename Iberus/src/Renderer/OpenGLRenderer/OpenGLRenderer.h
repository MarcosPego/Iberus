#pragma once

#include "Renderer.h"

namespace Iberus {

	class OpenGLRenderer : public Renderer {

		void RenderFrame(Frame& frame) override;
	};
}



