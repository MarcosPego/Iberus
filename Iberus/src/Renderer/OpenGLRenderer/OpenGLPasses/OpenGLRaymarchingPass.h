#pragma once

#include "RenderPass.h"

namespace Iberus {
	class MeshApi;

	class OpenGLRaymarchingPass : public RenderPass {
	public:
		OpenGLRaymarchingPass(Framebuffer* inSourceBuffer = nullptr, Framebuffer* inTargetBuffer = nullptr);

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) override;
	private:
		MeshApi* quadMesh{ nullptr };

		std::vector<int> texturesIdxs{ 4, 5, 6, 7 };
	};

}


