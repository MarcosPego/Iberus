#pragma once

#include "RenderPass.h"
#include <cstddef>
#include <vector>

/// std140 layout for SDF UBO. Must match baseRaymarchingShader.frag SDFBlock.
namespace SDFUBO {
	constexpr size_t SDF_PART_COUNT = 16;
	constexpr size_t SDF_MESH_COUNT = 4;
	constexpr size_t SDF_PART_STRIDE = 64;
	constexpr size_t MESH_HEADER_SIZE = 48;
	constexpr size_t MESH_STRIDE = MESH_HEADER_SIZE + SDF_PART_STRIDE * SDF_PART_COUNT;
	constexpr size_t BLOCK_SIZE = MESH_STRIDE * SDF_MESH_COUNT;
	constexpr size_t MESH_SIZE = 0;
	constexpr size_t MESH_BOUND_CENTER = 16;
	constexpr size_t MESH_BOUND_RADIUS = 32;
	constexpr size_t MESH_PARTS_START = 48;
	constexpr size_t PART_CENTER = 0;
	constexpr size_t PART_COLOR = 16;
	constexpr size_t PART_RADIUS = 32;
	constexpr size_t PART_TYPE = 36;
	constexpr size_t PART_ENDPOINT = 48;
}

namespace Iberus {
	class MeshApi;

	class OpenGLRaymarchingPass : public RenderPass {
	public:
		OpenGLRaymarchingPass(Framebuffer* inSourceBuffer = nullptr, Framebuffer* inTargetBuffer = nullptr);

		void ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) override;
	private:
		MeshApi* quadMesh{ nullptr };

		std::vector<int> texturesIdxs{ 4, 5, 6, 7 };
		static constexpr int depthTextureIdx = 8;
		static constexpr unsigned int sdfUBOBindingIndex = 0;

		unsigned int sdfUBO{ 0 };
	};

}


