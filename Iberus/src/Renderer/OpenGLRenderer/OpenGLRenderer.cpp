#include "Enginepch.h"
#include "OpenGLRenderer.h"

#include "OpenGLFramebuffer.h"
#include "OpenGLMesh.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "RenderBatch.h"
#include "RenderCmd.h"
#include "ShaderBindings.h"

namespace Iberus {
	OpenGLRenderer::OpenGLRenderer() {
		openGLTexBindings[0] = GL_TEXTURE0;
		openGLTexBindings[1] = GL_TEXTURE1;
		openGLTexBindings[2] = GL_TEXTURE2;
		openGLTexBindings[3] = GL_TEXTURE3;
		openGLTexBindings[4] = GL_TEXTURE4;
		openGLTexBindings[5] = GL_TEXTURE5;
		openGLTexBindings[6] = GL_TEXTURE6;
		openGLTexBindings[7] = GL_TEXTURE7;
		openGLTexBindings[8] = GL_TEXTURE8;
	}

	void OpenGLRenderer::RenderFrame(Frame& frame) {
		ExecuteAndFlushCmdQueue();

		auto _renderBatchCommands = [&](Frame& frame, ShaderApi* globalShader) {
			RenderBatchCommands(frame, globalShader);
		};

		for (const auto& pass : renderPasses) {
			pass->ExecutePass(frame, _renderBatchCommands);
		}
	}

	void OpenGLRenderer::ExecuteAndFlushCmdQueue() {
		for (const auto& renderCmd : renderCmdQueue) {
			switch (renderCmd->GetRenderCmdType()) {

			case RenderCmdType::UPLOAD_SHADER: {
				auto* shaderCmd = dynamic_cast<UploadShaderRenderCmd*>(renderCmd.get());
				auto handle = GenerateHandle(); // Needs to be reviewed
				auto* shader = new OpenGLShader(shaderCmd->ID, handle, std::move(shaderCmd->vertexBuffer), std::move(shaderCmd->fragBuffer));
				renderObjects[shaderCmd->ID].reset(shader);
			} break;
			case RenderCmdType::UPLOAD_MESH: {
				auto* meshCmd = dynamic_cast<UploadMeshRenderCmd*>(renderCmd.get());
				auto handle = GenerateHandle(); // Needs to be reviewed
				auto* mesh = new OpenGLMesh(meshCmd->ID, handle, meshCmd->vertices, meshCmd->uvs, meshCmd->normals);

				renderObjects[meshCmd->ID].reset(mesh);
			} break;
			case RenderCmdType::UPLOAD_TEXTURE: {
				auto* textureCmd = dynamic_cast<UploadTextureRenderCmd*>(renderCmd.get());
				auto handle = GenerateHandle(); // Needs to be reviewed
				auto* texture = new OpenGLTexture(textureCmd->ID, handle, std::move(textureCmd->buffer), textureCmd->width, textureCmd->height, textureCmd->channels);
				
				renderObjects[textureCmd->ID].reset(texture);
			} break;
			case RenderCmdType::DELETE_SHADER: {
				auto* shaderCmd = dynamic_cast<DeleteShaderRenderCmd*>(renderCmd.get());
				renderObjects.erase(shaderCmd->ID);
			} break;
			case RenderCmdType::DELETE_MESH: {
				auto* meshCmd = dynamic_cast<DeleteMeshRenderCmd*>(renderCmd.get());
				renderObjects.erase(meshCmd->ID);
			} break;
			case RenderCmdType::DELETE_TEXTURE: {
				auto* texCmd = dynamic_cast<DeleteTextureRenderCmd*>(renderCmd.get());
				renderObjects.erase(texCmd->ID); 
			} break;
			default:
				break;
			}
		}
		renderCmdQueue.clear();
	}

	Framebuffer* OpenGLRenderer::CreateFramebuffer(const std::string& ID, const std::vector<TextureApi*>& inTextures) {
		auto* framebuffer = new OpenGLFramebuffer(ID, inTextures);
		renderObjects[ID].reset(framebuffer);

		return dynamic_cast<Framebuffer*>(renderObjects[ID].get());
	}

	void OpenGLRenderer::Init() {
		renderPasses.emplace_back(new OpenGLForwardPass());
	}

	void OpenGLRenderer::RenderBatchCommands(Frame& frame, ShaderApi* globalShader) {
		static TextureApi* boundTexture{ nullptr };
		static MeshApi* boundMesh{ nullptr };
		static ShaderApi* shaderInUse{ nullptr };
		static Mat4 viewMatrix;
		static Mat4 projectionMatrix;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.15f, 0.15f, 0.15f, 0.3f);
		glEnable(GL_DEPTH_TEST);

		for (const RenderBatch& renderBatch : frame.renderBatches) {
			auto* cameraRenderCmd = renderBatch.GetCameraRenderCmd();
			if (cameraRenderCmd) {
				
				projectionMatrix = cameraRenderCmd->projectionMatrix;
				viewMatrix = cameraRenderCmd->viewMatrix;
			}

			if (globalShader != shaderInUse) {
				GLuint programID{ 0 };
				shaderInUse = globalShader;
				shaderInUse->Bind();
				if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderInUse); openGLShader) {
					programID = openGLShader->GetProgramID();
				}

				/// TODO(MPP) Make a enabled/disabled logger for the renderer for debug purposes
				//std::cout << "shader enabled" << std::endl;
				// Push Camera Uniforms
				ShaderBindings::SetUniform<Mat4>(programID, "ViewMatrix", viewMatrix);
				ShaderBindings::SetUniform<Mat4>(programID, "ProjectionMatrix", projectionMatrix);
				if (glGetError() != GL_NO_ERROR) {
					//std::cout << "Error in Shader" << std::endl;
				}
			}

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
					auto* shader = dynamic_cast<ShaderApi*>(renderObjects[shaderCmd->ID].get());

					if (shader == shaderInUse || globalShader) {
						continue;
					}

					if (shaderInUse) {
						shaderInUse->Unbind();
					}

					GLuint programID{ 0 };
					shaderInUse = shader;
					shaderInUse->Bind();
					if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderInUse); openGLShader) {
						programID = openGLShader->GetProgramID();
					}

					/// TODO(MPP) Make a enabled/disabled logger for the renderer for debug purposes
					//std::cout << "shader enabled" << std::endl;
					// Push Camera Uniforms
					ShaderBindings::SetUniform<Mat4>(programID, "ViewMatrix", viewMatrix);
					ShaderBindings::SetUniform<Mat4>(programID, "ProjectionMatrix", projectionMatrix);
					if (glGetError() != GL_NO_ERROR) {
						//std::cout << "Error in Shader" << std::endl;
					}
				}	break;
				case RenderCmdType::PUSH_MESH: {
					auto* meshCmd = dynamic_cast<MeshRenderCmd*>(renderCmd);
					auto* mesh = dynamic_cast<MeshApi*>(renderObjects[meshCmd->ID].get());

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
						//std::cout << "Error in Mesh" << std::endl;
					}
				}	break;

				case RenderCmdType::PUSH_TEXTURE: {
					auto* textureCmd = dynamic_cast<TextureRenderCmd*>(renderCmd);
					auto* texture = static_cast<TextureApi*>(renderObjects[textureCmd->ID].get());

					if (texture != boundTexture) {
						// Unbind previous texture
						if (boundTexture) {
							boundTexture->Unbind();
						}

						boundTexture = texture;
						glActiveTexture(openGLTexBindings[textureCmd->bind]);
						texture->Bind();


						/// TODO(MPP) Make a enabled/disabled logger for the renderer for debug purposes
						//std::cout << "texture enabled" << std::endl;
						if (glGetError() != GL_NO_ERROR) {
							//std::cout << "Error in texture" << std::endl;
						}
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
					PushUniform(renderCmd, programID);
				}	break;
				default:
					break;
				}
			}
		}

		if (boundTexture) {
			boundTexture->Unbind();
		}

		if (boundMesh) {
			boundMesh->Unbind();
		}

		if (shaderInUse) {
			shaderInUse->Unbind();
		}

		boundTexture = nullptr;
		boundMesh = nullptr;
		shaderInUse = nullptr;
	}
}