#pragma once

#include "RenderPass.h"

namespace Iberus {
	class MeshApi;
	class Framebuffer;

	class OpenGLDeferredLightPass : public RenderPass {
	public:
		OpenGLDeferredLightPass();

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) override;
	private:
		Framebuffer* frameBuffer{ nullptr };
		MeshApi* quadMesh{ nullptr };
	};

}


