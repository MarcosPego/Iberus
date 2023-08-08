#include "Enginepch.h"
#include "OpenGLRenderer.h"

#include "OpenGLMesh.h"
#include "OpenGLShader.h"
#include "RenderBatch.h"
#include "RenderCmd.h"
#include "ShaderBindings.h"

namespace Iberus {
	void OpenGLRenderer::RenderFrame(Frame& frame) {
		static MeshApi* boundMesh{ nullptr };
		static ShaderApi* shaderInUse{ nullptr };
		static Mat4 viewMatrix;
		static Mat4 projectionMatrix;

		ExecuteAndFlushCmdQueue();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.15f, 0.15f, 0.15f, 0.3f);
		glEnable(GL_DEPTH_TEST);

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
					auto* shader = dynamic_cast<ShaderApi*>(renderObjects[shaderCmd->shaderID].get());

					if (shader == shaderInUse) {
						continue;
					}

					if (shaderInUse) {
						shaderInUse->Disable();
					}

					GLuint programID{ 0 };
					shaderInUse = shader;
					shaderInUse->Enable();
					if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderInUse); openGLShader) {
						programID = openGLShader->GetProgramID();
					}

					// Push Camera Uniforms
					ShaderBindings::SetUniform<Mat4>(programID, "ViewMatrix", viewMatrix);
					ShaderBindings::SetUniform<Mat4>(programID, "ProjectionMatrix", projectionMatrix);
					ShaderBindings::SetUniform<Vec4>(programID, "Color", Vec4(0.0f, 0.0f, 1.0f, 1.0f));

					if (glGetError() != GL_NO_ERROR) {
						std::cout << "Error in Shader" << std::endl;
					}
				}	break;
				case RenderCmdType::PUSH_MESH: {
					auto* meshCmd = dynamic_cast<MeshRenderCmd*>(renderCmd);
					auto mesh = dynamic_cast<MeshApi*>(renderObjects[meshCmd->meshID].get());
					
					if (mesh != boundMesh) {
						// Unbind previous mesh
						if (boundMesh) {
							boundMesh->Unbind();
						}

						boundMesh = mesh;

						mesh->Bind();
					}
					glDrawArrays(GL_TRIANGLES, 0, (GLsizei)mesh->VertexSize());
					if (glGetError() != GL_NO_ERROR) {
						std::cout << "Error in Mesh" << std::endl;
					}
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
	
		if (boundMesh) {
			boundMesh->Unbind();
		}

		if (shaderInUse) {
			shaderInUse->Disable();
		}

		boundMesh = nullptr;
		shaderInUse = nullptr;
	}

	void OpenGLRenderer::ExecuteAndFlushCmdQueue() {
		for (const auto& renderCmd : renderCmdQueue) {
			switch (renderCmd->GetRenderCmdType()) {

			case RenderCmdType::UPLOAD_SHADER: {
				auto* shaderCmd = dynamic_cast<UploadShaderRenderCmd*>(renderCmd.get());
				auto handle = GenerateHandle(); // Needs to be reviewed
				auto* shader = new OpenGLShader(shaderCmd->shaderID, handle, std::move(shaderCmd->vertexBuffer), std::move(shaderCmd->fragBuffer));
				renderObjects[shaderCmd->shaderID].reset(shader);
			} break;
			case RenderCmdType::UPLOAD_MESH: {
				auto* meshCmd = dynamic_cast<UploadMeshRenderCmd*>(renderCmd.get());
				auto handle = GenerateHandle(); // Needs to be reviewed
				auto* mesh = new OpenGLMesh(meshCmd->meshID, handle, meshCmd->vertices, meshCmd->uvs, meshCmd->normals);

				renderObjects[meshCmd->meshID].reset(mesh);
			} break;
			case RenderCmdType::UPLOAD_TEXTURE: {} break;
			case RenderCmdType::DELETE_SHADER: {
				auto* shaderCmd = dynamic_cast<DeleteShaderRenderCmd*>(renderCmd.get());
				renderObjects.erase(shaderCmd->shaderID);
			} break;
			case RenderCmdType::DELETE_MESH: {
				auto* meshCmd = dynamic_cast<DeleteMeshRenderCmd*>(renderCmd.get());
				renderObjects.erase(meshCmd->meshID);
			} break;
			case RenderCmdType::DELETE_TEXTURE: {} break;
			default:
				break;
			}
			

			//delete renderCmd;
		}
		renderCmdQueue.clear();
	}
}