#include "Enginepch.h"
#include "Renderer.h"

#include "OpenGLRenderer.h"
#include "OpenGLDeferredRenderer.h"
#include "ShaderBindings.h"

namespace Iberus {

	Renderer* Renderer::Create() {
		return new OpenGLRenderer();
	}

	Renderer* Renderer::CreateDeferred() {
		return new OpenGLDeferredRenderer();
	}

	Renderer::~Renderer() {
	}

	void Renderer::PushRenderCmd(RenderCmd* renderCmd) {
		renderCmdQueue.emplace_back(renderCmd);
	}

	void Renderer::ExecuteAndFlushCmdQueue() {
		renderCmdQueue.clear();
	}

	void Renderer::PushUniform(RenderCmd* renderCmd, int programID) {
		switch (renderCmd->GetUniformType())
		{
		case UniformType::INT: {
			auto uniformRenderCmd = dynamic_cast<UniformRenderCmd<int>*>(renderCmd);
			ShaderBindings::SetUniform<int>(programID, uniformRenderCmd->GetName().c_str(), uniformRenderCmd->GetValue());
		} break;
		case UniformType::FLOAT: {
			auto uniformRenderCmd = dynamic_cast<UniformRenderCmd<float>*>(renderCmd);
			ShaderBindings::SetUniform<float>(programID, uniformRenderCmd->GetName().c_str(), uniformRenderCmd->GetValue());
		} break;
		case UniformType::VEC2: {
			auto uniformRenderCmd = dynamic_cast<UniformRenderCmd<Vec2>*>(renderCmd);
			ShaderBindings::SetUniform<Vec2>(programID, uniformRenderCmd->GetName().c_str(), uniformRenderCmd->GetValue());
		} break;
		case UniformType::VEC3: {
			auto uniformRenderCmd = dynamic_cast<UniformRenderCmd<Vec3>*>(renderCmd);
			ShaderBindings::SetUniform<Vec3>(programID, uniformRenderCmd->GetName().c_str(), uniformRenderCmd->GetValue());
		} break;
		case UniformType::VEC4: {
			auto uniformRenderCmd = dynamic_cast<UniformRenderCmd<Vec4>*>(renderCmd);
			ShaderBindings::SetUniform<Vec4>(programID, uniformRenderCmd->GetName().c_str(), uniformRenderCmd->GetValue());
		} break;
		case UniformType::MAT4: {
			auto uniformRenderCmd = dynamic_cast<UniformRenderCmd<Mat4>*>(renderCmd);
			ShaderBindings::SetUniform<Mat4>(programID, uniformRenderCmd->GetName().c_str(), uniformRenderCmd->GetValue());
		} break;
		default:
			break;
		}
	}

	RenderObject* Renderer::GetResource(const std::string& ID) const {
		if (renderObjects.find(ID) != renderObjects.end()) {
			return renderObjects.at(ID).get();
		}
		return nullptr;
	}

	uint32_t Renderer::GenerateHandle() {
		return static_cast<uint32_t>(renderObjects.size());
	}
}