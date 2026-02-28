#pragma once

#include "RenderPass.h"
#include "Matrix.h"
#include "RenderCmd.h"
#include <cstddef>
#include <vector>

/// std430 SSBO layout. Must match baseRaymarchingShader.frag SDFBlock.
namespace SDFSSBO {
	constexpr size_t POOL_BUDGET = 4096;
	constexpr size_t MAX_CREATURES = 256;
	constexpr size_t SDF_PART_STRIDE = 80;
	constexpr size_t CREATURE_HEADER_STRIDE = 48;  // std430: 2*int + 8 pad + vec4 + float + int
	constexpr size_t CREATURE_HEADER_PART_OFFSET = 0;
	constexpr size_t CREATURE_HEADER_PART_COUNT = 4;
	constexpr size_t CREATURE_HEADER_BOUND_CENTER = 16;  // vec4 must be 16-byte aligned
	constexpr size_t CREATURE_HEADER_BOUND_RADIUS = 32;
	constexpr size_t PARTS_OFFSET = 16 + MAX_CREATURES * CREATURE_HEADER_STRIDE;
	constexpr size_t PART_CENTER = 0;
	constexpr size_t PART_COLOR = 16;
	constexpr size_t PART_RADIUS = 32;
	constexpr size_t PART_TYPE = 36;
	constexpr size_t PART_BLEND_GROUP_MASK = 40;
	constexpr size_t PART_ENDPOINT = 48;
	constexpr size_t PART_VOLUME_TEXTURE = 64;  // vec4 placeholder for future volume texture
	constexpr size_t BLOCK_SIZE = PARTS_OFFSET + POOL_BUDGET * SDF_PART_STRIDE;
	constexpr size_t GLOBAL_HEADER_SIZE = 16;
	constexpr size_t GLOBAL_CREATURE_COUNT = 0;
	constexpr size_t GLOBAL_TOTAL_PART_COUNT = 4;
}

namespace Iberus {
	class MeshApi;
	class ComputeShaderApi;

	class OpenGLRaymarchingPass : public RenderPass {
	public:
		OpenGLRaymarchingPass();
		~OpenGLRaymarchingPass();

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame, Framebuffer* source = nullptr, Framebuffer* target = nullptr) override;
		std::string GetName() const override { return "Raymarching"; }
	private:
		void BuildTileBuffer(const SDFBufferRenderCmd* sdfCmd, const Math::Mat4& viewProj, int screenW, int screenH);
		void BuildTileBufferGPU(const Math::Mat4& viewProj, int screenW, int screenH);
		void BuildEmptyTileBuffer(int screenW, int screenH);

		// ExecutePass pipeline stages - each does one thing for easier debugging
		void SetupCameraAndSDFData(Frame& frame, int w, int h, GLuint programID,
			SDFBufferRenderCmd*& outSdfCmd, Math::Mat4& outViewProj, Math::Mat4& outView, Math::Mat4& outProj);
		void SetupTileBuffer(const SDFBufferRenderCmd* sdfCmd, const Math::Mat4& viewProj, int w, int h);
		bool RunCoarsePass(int w, int h, const SDFBufferRenderCmd* sdfCmd,
			const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix);
		void ExecuteCoarseToTarget(int w, int h, Framebuffer* source, Framebuffer* target,
			const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix);
		void ExecuteDirectTest(int w, int h, Framebuffer* source, Framebuffer* target);
		void ExecuteBlit(int w, int h, Framebuffer* source, Framebuffer* target);
		void ExecuteCoarseFramebufferBlit(int w, int h, Framebuffer* target);
		void ExecuteMainRaymarch(int w, int h, Framebuffer* source, Framebuffer* target, GLuint programID,
			bool useCoarseMask, bool showCoarseDebug);

		static constexpr int TILE_SIZE = 32;
		static constexpr int MAX_SDF_PER_TILE = 64;
		static constexpr int TILE_STRIDE = 1 + MAX_SDF_PER_TILE;  // count + indices

		MeshApi* quadMesh{ nullptr };
		ShaderApi* coarseShaderPass{ nullptr };
		ShaderApi* coarseDebugBlitPass{ nullptr };
		ShaderApi* coarseDirectTestPass{ nullptr };
		ComputeShaderApi* tileBufferComputePass{ nullptr };

		std::vector<int> texturesIdxs{ 4, 5, 6, 7 };
		static constexpr int depthTextureIdx = 8;
		static constexpr int tileBufferTextureIdx = 9;
		static constexpr int coarseMaskTextureIdx = 10;
		static constexpr unsigned int sdfSSBOBindingIndex = 0;

		unsigned int sdfSSBO{ 0 };
		unsigned int tileBuffer{ 0 };
		unsigned int tileBufferTexture{ 0 };
		unsigned int coarseFBO{ 0 };
		unsigned int coarseTexture{ 0 };
		int lastTileBufferBytes{ 0 };
		int coarseWidth{ 0 };
		int coarseHeight{ 0 };
		size_t sdfSSBOCapacity{ 0 };
		static constexpr int COARSE_SCALE = 8;
		static constexpr unsigned int tileBufferSSBOBindingIndex = 1;
	};

}


