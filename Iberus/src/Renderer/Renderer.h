#pragma once

#include "RenderBatch.h"
#include "MathUtils.h"

#include "RenderCmd.h"
#include "RenderObject.h"

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

		virtual ~Renderer();

		virtual void PushRenderCmd(RenderCmd* renderCmd);
		virtual void RenderFrame(Frame& frame) = 0;
		virtual void ExecuteAndFlushCmdQueue();

		uint32_t GenerateHandle();

	protected:
		std::unordered_map<std::string, std::unique_ptr<RenderObject>> renderObjects;
		std::vector<std::unique_ptr<RenderCmd>> renderCmdQueue; // This queue is meant for other render operations besides push drawing
	};
}


