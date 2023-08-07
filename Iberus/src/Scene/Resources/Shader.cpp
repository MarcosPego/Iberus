#include "Enginepch.h"
#include "Shader.h"

#include "Engine.h"
#include "Renderer.h"
#include "RenderCmd.h"

namespace Iberus {

	Shader::Shader(const std::string& inboundID, Buffer vertexBuffer, Buffer fragBuffer) : Resource(inboundID) {
		auto& renderer = Engine::Instance()->GetRenderer();
		auto* uploadShader = new UploadShaderRenderCmd(ID, std::move(vertexBuffer), std::move(fragBuffer));
		renderer.PushRenderCmd(uploadShader);
	}

	Shader::~Shader() {
		auto& renderer = Engine::Instance()->GetRenderer();
		auto deleteShader = new DeleteShaderRenderCmd(ID);
		renderer.PushRenderCmd(deleteShader);
	}

}