#pragma once

#include "Renderer.h"

namespace Iberus {

	class OpenGLRenderer : public Renderer {
	public:
		OpenGLRenderer();

		void RenderFrame(Frame& frame) override;
		void ExecuteAndFlushCmdQueue() override;
	private:

		std::unordered_map<int, int> openGLTexBindings;
	};
}



