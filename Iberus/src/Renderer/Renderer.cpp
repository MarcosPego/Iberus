#include "Enginepch.h"
#include "Renderer.h"

#include "OpenGLRenderer.h"

namespace Iberus {

	Renderer* Renderer::Create() {
		return new OpenGLRenderer();
	}

	Renderer::~Renderer() {
		for (auto* renderCmd : renderCmdQueue) {
			delete renderCmd;
		}
	}

	void Renderer::PushRenderCmd(RenderCmd* renderCmd) {
		renderCmdQueue.push_back(renderCmd);
	}

	void Renderer::ExecuteAndFlushCmdQueue() {
		for (auto* renderCmd : renderCmdQueue) {
			delete renderCmd;
		}
		renderCmdQueue.clear();
	}
}