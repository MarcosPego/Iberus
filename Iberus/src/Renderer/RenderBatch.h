#pragma once

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
		~RenderBatch();

		void PushRenderCmdToQueue(RenderCmd* inboundRenderCmd, CMDQueue queue = CMDQueue::Default);
	
		const std::vector<RenderCmd*>& GetLightRenderCmd() const {
			return lightRenderCmds;
		}

		const std::vector<RenderCmd*>& GetSDFRenderCmds() const {
			return sdfRenderCmds;
		}

		const CameraRenderCmd* GetCameraRenderCmd() const {
			return cameraCmd;
		}

		const std::vector<RenderCmd*>& GetRenderCmds() const {
			return renderCmds;
		}

	private:
		std::vector<RenderCmd*> renderCmds;

		std::vector<RenderCmd*> sdfRenderCmds;
		std::vector<RenderCmd*> lightRenderCmds;

		CameraRenderCmd* cameraCmd{ nullptr };
	};
}


