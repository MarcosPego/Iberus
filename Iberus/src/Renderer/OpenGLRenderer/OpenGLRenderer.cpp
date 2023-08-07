#include "Enginepch.h"
#include "OpenGLRenderer.h"

#include "OpenGLShader.h"
#include "RenderBatch.h"
#include "RenderCmd.h"
#include "ShaderBindings.h"

namespace Iberus {
	void OpenGLRenderer::RenderFrame(Frame& frame) {
		static ShaderApi* shaderInUse{ nullptr };
		static Mat4 viewMatrix;
		static Mat4 projectionMatrix;

		ExecuteAndFlushCmdQueue();

		for (const RenderBatch& renderBatch : frame.renderBatches) {
			const auto& renderCmds = renderBatch.GetRenderCmds();
			for (const auto& renderCmd : renderCmds) {

				switch (renderCmd->GetRenderCmdType()) {
				case RenderCmdType::PUSH_CAMERA: {
					auto* cameraCmd = dynamic_cast<CameraRenderCmd*>(renderCmd);
					projectionMatrix = cameraCmd->projectionMatrix;
					viewMatrix = cameraCmd->viewMatrix;
				}	break;
				case RenderCmdType::PUSH_SHADER: {
					auto* shaderCmd = dynamic_cast<ShaderRenderCmd*>(renderCmd);
					GLuint programID{ 0 };
					// TODO()
					/*auto* shader = shaderCmd->shader;
					shaderInUse = shader;
					shaderInUse->Enable();

					
					if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderInUse); openGLShader) {
						programID = openGLShader->GetProgramID();
					}*/

					// Push Camera Uniforms
					ShaderBindings::SetUniform<Mat4>(programID, "ViewMatrix", viewMatrix);
					ShaderBindings::SetUniform<Mat4>(programID, "ProjectionMatrix", projectionMatrix);
				}	break;
				case RenderCmdType::PUSH_MESH: {
					auto* meshCmd = dynamic_cast<MeshRenderCmd*>(renderCmd);
					// TODO()
					//auto* mesh = meshCmd->mesh;
					//mesh->Bind();
					//glDrawArrays(GL_TRIANGLES, 0, (GLsizei)mesh->VertexSize());
					//mesh->Unbind();
				}	break;
				case RenderCmdType::PUSH_UNIFORM: {
					GLuint programID{ 0 };
					if (!shaderInUse) {
						continue;
					}

					if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderInUse); openGLShader) {
						programID = openGLShader->GetProgramID();
					}

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
	
		if (shaderInUse) {
			shaderInUse->Disable();
		}
		shaderInUse = nullptr;
	}

	void OpenGLRenderer::ExecuteAndFlushCmdQueue() {
		for (auto* renderCmd : renderCmdQueue) {
			switch (renderCmd->GetRenderCmdType()) {

			case RenderCmdType::UPLOAD_SHADER: {} break;
			case RenderCmdType::UPLOAD_MESH: {} break;
			case RenderCmdType::UPLOAD_TEXTURE: {} break;
			case RenderCmdType::DELETE_SHADER: {} break;
			case RenderCmdType::DELETE_MESH: {} break;
			case RenderCmdType::DELETE_TEXTURE: {} break;
			default:
				break;
			}
			

			delete renderCmd;
		}
		renderCmdQueue.clear();
	}
}