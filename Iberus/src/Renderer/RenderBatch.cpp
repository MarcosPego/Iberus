#include "Enginepch.h"
#include "RenderBatch.h"

namespace Iberus {

	RenderBatch::~RenderBatch() {
		for (auto* renderCmd : renderCmds) {
			delete renderCmd;
		}
	}

	void RenderBatch::PushRenderCmd(RenderCmd* inboundRenderCmd) {
		renderCmds.push_back(inboundRenderCmd);
	}

}