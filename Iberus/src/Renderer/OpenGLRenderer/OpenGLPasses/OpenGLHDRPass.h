#pragma once
#include "RenderPass.h"

namespace Iberus {
	class MeshApi;

	class OpenGLHDRPass : public RenderPass {
	public:
		OpenGLHDRPass();
		~OpenGLHDRPass();

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame, Framebuffer* source = nullptr, Framebuffer* target = nullptr) override;
		std::string GetName() const override { return "HDR"; }

	private:
		void EnsureBloomBuffers(int width, int height);
		void ReleaseBloomBuffers();

		MeshApi* quadMesh{ nullptr };
		ShaderApi* brightPassShader{ nullptr };
		ShaderApi* blurShader{ nullptr };

		unsigned int bloomFBO1{ 0 };
		unsigned int bloomFBO2{ 0 };
		unsigned int bloomTex1{ 0 };
		unsigned int bloomTex2{ 0 };
		int bloomWidth{ 0 };
		int bloomHeight{ 0 };

		float adaptationSpeed = 1;
		float autoExposureMultiplier = 5;
		float exposure = 1.0f;
		float gamma = 1.0f;
		float bloomThreshold = 1.0f;
		float bloomIntensity = 0.0f;
		float bloomSpread = 24.0f;
		std::vector<int> texturesIdxs{ 4, 5, 6, 7 };
		static constexpr int bloomTextureUnit = 8;
	};
}



