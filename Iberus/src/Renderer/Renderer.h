#pragma once

#include "RenderBatch.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	class RenderCmd;
	class RenderOject;


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

		virtual ~Renderer();

		virtual void PushRenderCmd(RenderCmd* renderCmd);
		virtual void RenderFrame(Frame& frame) = 0;
		virtual void ExecuteAndFlushCmdQueue();

	protected:
		std::vector<RenderOject*> renderObjects;
		std::vector<RenderCmd*> renderCmdQueue; // This queue is meant for other render operations besides push drawing
	};
}


