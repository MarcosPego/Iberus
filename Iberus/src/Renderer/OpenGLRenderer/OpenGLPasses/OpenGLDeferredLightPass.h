#pragma once

#include "RenderPass.h"

namespace Iberus {
	class MeshApi;

	class OpenGLDeferredLightPass : public RenderPass {
	public:
		OpenGLDeferredLightPass(Framebuffer* inSourceFBO = nullptr, Framebuffer* inTargetFBO = nullptr);

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) override;
	private:
		MeshApi* quadMesh{ nullptr };
		
		std::vector<int> texturesIdxs{ 4, 5, 6, 7 };
	};

}


