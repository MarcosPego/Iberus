#pragma once

namespace Iberus {
	class CameraRenderCmd;
	class RenderCmd;

	class RenderBatch {
	public:
		~RenderBatch();

		void PushCameraRenderCmd(RenderCmd* inboundRenderCmd);
		
		void PushRenderCmd(RenderCmd* inboundRenderCmd);
			
		const CameraRenderCmd* GetCameraRenderCmd() const {
			return cameraCmd;
		}

		const std::vector<RenderCmd*>& GetRenderCmds() const {
			return renderCmds;
		}

	private:
		std::vector<RenderCmd*> renderCmds;
		CameraRenderCmd* cameraCmd{ nullptr };
	};
}


