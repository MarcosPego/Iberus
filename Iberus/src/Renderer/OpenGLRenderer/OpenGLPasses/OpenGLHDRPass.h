#pragma once
#include "RenderPass.h"

namespace Iberus {
	class MeshApi;

	class OpenGLHDRPass : public RenderPass {
	public:
		OpenGLHDRPass(Framebuffer* inSourceBuffer = nullptr, Framebuffer* inTargetBuffer = nullptr);

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) override;

	private:
		MeshApi* quadMesh{ nullptr };

		float adaptationSpeed = 1;
		float autoExposureMultiplier = 5;
		float exposure = 1.0f;
		float gamma = 1.0f;
		std::vector<int> texturesIdxs{ 4, 5, 6, 7 };
	};
}



