#include "Enginepch.h"
#include "OpenGLRenderer.h"

#include "RenderBatch.h"
#include "RenderCmd.h"
#include "ShaderBindings.h"

namespace Iberus {
	void OpenGLRenderer::RenderFrame(Frame& frame) {
		static GLuint programID{ 0 };

		for (const RenderBatch& renderBatch : frame.renderBatches) {
			const auto& renderCmds = renderBatch.GetRenderCmds();
			for (const auto& renderCmd : renderCmds) {

				switch (renderCmd->GetRenderCmdType()) {
				case RenderCmdType::PUSH_MESH: {
					// TODO(MPP) Not sure this even works;
					dynamic_cast<MeshRenderCmd*>(renderCmd);
				}	break;

				case RenderCmdType::PUSH_UNIFORM: {
					// TODO(MPP) Not sure this even works;
					//auto uniformRenderCmd = dynamic_cast<UniformRenderCmd<decltype(renderCmd->type.type())>*>(renderCmd);

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
				
				}	break;
				default:
					break;
				}

			}
		}
	}
}