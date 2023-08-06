#pragma once

#include "RenderCmd.h"

namespace Iberus {
	class RenderBatch {
	public:
		~RenderBatch() {
			for (auto* renderCmd : renderCmds) {
				delete renderCmd;
			}
		}

		void PushRenderCmd(RenderCmd* inboundRenderCmd);
			
		const std::vector<RenderCmd*>& GetRenderCmds() const {
			return renderCmds;
		}

	private:
		std::vector<RenderCmd*> renderCmds;
	};
}


