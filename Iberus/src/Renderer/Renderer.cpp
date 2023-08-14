#include "Enginepch.h"
#include "Renderer.h"

#include "OpenGLRenderer.h"
#include "OpenGLDeferredRenderer.h"


namespace Iberus {

	Renderer* Renderer::Create() {
		return new OpenGLRenderer();
	}

	Renderer* Renderer::CreateDeferred() {
		return new OpenGLDeferredRenderer();
	}

	Renderer::~Renderer() {
	}

	void Renderer::PushRenderCmd(RenderCmd* renderCmd) {
		renderCmdQueue.emplace_back(renderCmd);
	}

	void Renderer::ExecuteAndFlushCmdQueue() {
		renderCmdQueue.clear();
	}

	RenderObject* Renderer::GetResource(const std::string& ID) const {
		if (renderObjects.find(ID) != renderObjects.end()) {
			return renderObjects.at(ID).get();
		}
		return nullptr;
	}

	uint32_t Renderer::GenerateHandle() {
		return static_cast<uint32_t>(renderObjects.size());
	}
}