#pragma once

#include "RenderBatch.h"
#include "MathUtils.h"

#include "RenderCmd.h"
#include "RenderObject.h"

#include "ShaderApi.h"
#include "TextureApi.h"
#include "MeshApi.h"

using namespace Math;

namespace Iberus {
	struct Frame {
		Vec4 clearColor{ 0.15f, 0.15f, 0.17f, 1.0f };
		std::vector<RenderBatch> renderBatches;

		RenderBatch& PushBatch() {
			renderBatches.emplace_back(RenderBatch()); // Push a new Render Batch
			return renderBatches.back();
		}
	};

	class Renderer {
	public:
		static Renderer* Renderer::Create();
		static Renderer* Renderer::CreateDeferred();

		virtual ~Renderer();

		virtual void Init() = 0;

		virtual void PushRenderCmd(RenderCmd* renderCmd);
		virtual void RenderFrame(Frame& frame) = 0;
		virtual void ExecuteAndFlushCmdQueue();

		uint32_t GenerateHandle();

		RenderObject* GetResource(const std::string& ID) const;

	protected:
		std::unordered_map<std::string, std::unique_ptr<RenderObject>> renderObjects;
		std::vector<std::unique_ptr<RenderCmd>> renderCmdQueue; // This queue is meant for other render operations besides push drawing
	};
}


