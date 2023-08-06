#pragma once

#include "RenderBatch.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	struct Frame {
		Vec4 clearColor{ 0.0f, 0.0f, 0.0f, 0.0f };
		std::vector<RenderBatch> renderBatch;
	};

	class Renderer {
	public:

		virtual void RenderFrame(Frame& frame) = 0;
	private:
	};
}


