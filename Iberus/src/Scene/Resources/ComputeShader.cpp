#include "Enginepch.h"
#include "ComputeShader.h"
#include "Engine.h"
#include "Renderer.h"
#include "RenderCmd.h"

namespace Iberus {

	ComputeShader::ComputeShader(const std::string& inboundID, Buffer compBuffer) : Resource(inboundID) {
		auto* uploadCmd = new UploadComputeShaderRenderCmd(ID, std::move(compBuffer));
		Engine::Instance()->GetRenderer().PushRenderCmd(uploadCmd);
	}

	ComputeShader::~ComputeShader() {
		auto* deleteCmd = new DeleteComputeShaderRenderCmd(ID);
		Engine::Instance()->GetRenderer().PushRenderCmd(deleteCmd);
	}

}
