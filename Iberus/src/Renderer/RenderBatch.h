#pragma once

namespace Iberus {
	class RenderCmd;

	class RenderBatch {
	public:
		~RenderBatch();

		void PushRenderCmd(RenderCmd* inboundRenderCmd);
			
		const std::vector<RenderCmd*>& GetRenderCmds() const {
			return renderCmds;
		}

	private:
		std::vector<RenderCmd*> renderCmds;
	};
}


