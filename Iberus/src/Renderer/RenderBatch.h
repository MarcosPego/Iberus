#pragma once

#include <memory>
#include <vector>

namespace Iberus {
	class CameraRenderCmd;
	class RenderCmd;

	enum class CMDQueue {
		Default,
		Camera,
		SDF,
		Light
	};

	class RenderBatch {
	public:
		void PushRenderCmdToQueue(std::unique_ptr<RenderCmd> inboundRenderCmd, CMDQueue queue = CMDQueue::Default);
	
		const std::vector<std::unique_ptr<RenderCmd>>& GetLightRenderCmd() const {
			return lightRenderCmds;
		}

		const std::vector<std::unique_ptr<RenderCmd>>& GetSDFRenderCmds() const {
			return sdfRenderCmds;
		}

		const CameraRenderCmd* GetCameraRenderCmd() const {
			return cameraCmd.get();
		}

		const std::vector<std::unique_ptr<RenderCmd>>& GetRenderCmds() const {
			return renderCmds;
		}

	private:
		std::vector<std::unique_ptr<RenderCmd>> renderCmds;

		std::vector<std::unique_ptr<RenderCmd>> sdfRenderCmds;
		std::vector<std::unique_ptr<RenderCmd>> lightRenderCmds;

		std::unique_ptr<CameraRenderCmd> cameraCmd;
	};
}


