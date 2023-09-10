#include "Enginepch.h"
#include "RenderBatch.h"

#include "RenderCmd.h"

namespace Iberus {

	RenderBatch::~RenderBatch() {
		if (cameraCmd) {
			delete cameraCmd;
		}


		for (auto* renderCmd : renderCmds) {
			delete renderCmd;
		}
	}

	void RenderBatch::PushRenderCmdToQueue(RenderCmd* inboundRenderCmd, CMDQueue queue) {
		switch (queue)
		{
		case Iberus::CMDQueue::Default:
			renderCmds.push_back(inboundRenderCmd);
			break;
		case Iberus::CMDQueue::Camera:
			cameraCmd = dynamic_cast<CameraRenderCmd*>(inboundRenderCmd);
			break;
		case Iberus::CMDQueue::SDF:
			sdfRenderCmds.push_back(inboundRenderCmd);
			break;
		case Iberus::CMDQueue::Light:
			lightRenderCmds.push_back(inboundRenderCmd);
			break;
		default:
			break;
		}

		
	}
}