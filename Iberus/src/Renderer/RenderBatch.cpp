#include "Enginepch.h"
#include "RenderBatch.h"

#include "RenderCmd.h"

namespace Iberus {

	RenderBatch::~RenderBatch() {
		delete cameraCmd;

		for (auto* renderCmd : renderCmds) {
			delete renderCmd;
		}
	}

	void RenderBatch::PushCameraRenderCmd(RenderCmd* inboundRenderCmd) {
		cameraCmd = dynamic_cast<CameraRenderCmd*>(inboundRenderCmd);
	}

	void RenderBatch::PushRenderCmd(RenderCmd* inboundRenderCmd) {
		renderCmds.push_back(inboundRenderCmd);
	}

}