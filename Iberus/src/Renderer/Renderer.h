#pragma once

#include "RenderBatch.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	struct Frame {
		Vec4 clearColor{ 0.0f, 0.0f, 0.0f, 0.0f };
		std::vector<RenderBatch> renderBatches;

		RenderBatch& PushBatch() {
			renderBatches.emplace_back(RenderBatch()); // Push a new Render Batch
			return renderBatches.back();
		}
	};

	class Renderer {
	public:
		static Renderer* Renderer::Create();


		virtual void RenderFrame(Frame& frame) = 0;
	private:
	};
}


