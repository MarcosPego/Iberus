#include "Enginepch.h"
#include "RenderBatch.h"

namespace Iberus {

	void RenderBatch::PushRenderCmd(RenderCmd* inboundRenderCmd) {
		renderCmds.push_back(inboundRenderCmd);
	}

}