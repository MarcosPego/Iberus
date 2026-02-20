#include "Enginepch.h"
#include "RenderBatch.h"

#include "RenderCmd.h"

namespace Iberus {

	void RenderBatch::PushRenderCmdToQueue(std::unique_ptr<RenderCmd> inboundRenderCmd, CMDQueue queue) {
		switch (queue)
		{
		case Iberus::CMDQueue::Default:
			renderCmds.push_back(std::move(inboundRenderCmd));
			break;
		case Iberus::CMDQueue::Camera:
			cameraCmd.reset(dynamic_cast<CameraRenderCmd*>(inboundRenderCmd.release()));
			break;
		case Iberus::CMDQueue::SDF:
			sdfRenderCmds.push_back(std::move(inboundRenderCmd));
			break;
		case Iberus::CMDQueue::Light:
			lightRenderCmds.push_back(std::move(inboundRenderCmd));
			break;
		default:
			break;
		}
	}
}