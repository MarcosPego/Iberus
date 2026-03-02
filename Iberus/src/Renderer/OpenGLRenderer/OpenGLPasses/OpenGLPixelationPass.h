#pragma once

#include "RenderPass.h"

namespace Iberus {
	class MeshApi;

	class OpenGLPixelationPass : public RenderPass {
	public:
		OpenGLPixelationPass();

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame, Framebuffer* source = nullptr, Framebuffer* target = nullptr) override;
		std::string GetName() const override { return "Pixelation"; }

	private:
		MeshApi* quadMesh{ nullptr };
		std::vector<int> texturesIdxs{ 4, 5, 6, 7 };
	};

}
