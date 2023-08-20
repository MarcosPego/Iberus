#pragma once

#include "RenderPass.h"

namespace Iberus {

	class MeshApi;
	class Framebuffer;

	class OpenGLRaymarchingPass : public RenderPass {
	public:
		OpenGLRaymarchingPass();

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) override;
	private:
		Framebuffer* sourceBuffer{ nullptr };
		Framebuffer* targetBuffer{ nullptr };
		MeshApi* quadMesh{ nullptr };

		std::vector<std::string> textures;
	};

}


