#include "Enginepch.h"
#include "OpenGLRaymarchingPass.h"

#include "Engine.h"
#include "Framebuffer.h"
#include "Matrix.h"
#include "ShaderBindings.h"
#include "OpenGLShader.h"
#include "OpenGLFramebuffer.h"
#include "RenderCmd.h"

#include "ShaderApi.h"
#include "TextureApi.h"
#include "MeshApi.h"

#include <algorithm>
#include <cstring>

using namespace Math;

namespace Iberus {

	static void ProjectToScreen(const Vec3& worldPos, const Mat4& viewProj, int screenW, int screenH,
		float& outX, float& outY, float& outW) {
		Vec4 clip = viewProj * Vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
		outW = clip.w;
		if (std::abs(clip.w) < 1e-6f) {
			outX = -1e6f;
			outY = -1e6f;
			return;
		}
		float ndcX = clip.x / clip.w;
		float ndcY = clip.y / clip.w;
		outX = (ndcX + 1.0f) * 0.5f * static_cast<float>(screenW);
		outY = (ndcY + 1.0f) * 0.5f * static_cast<float>(screenH);
	}

	void OpenGLRaymarchingPass::BuildTileBuffer(const SDFBufferRenderCmd* sdfCmd, const Math::Mat4& viewProj, int screenW, int screenH) {
		const int tilesX = (screenW + TILE_SIZE - 1) / TILE_SIZE;
		const int tilesY = (screenH + TILE_SIZE - 1) / TILE_SIZE;
		const int tileCount = tilesX * tilesY;
		const int bufferSize = tileCount * TILE_STRIDE * static_cast<int>(sizeof(uint32_t));

		if (tileBuffer == 0 || bufferSize != lastTileBufferBytes) {
			if (tileBuffer != 0) {
				glDeleteBuffers(1, &tileBuffer);
				glDeleteTextures(1, &tileBufferTexture);
			}
			glGenBuffers(1, &tileBuffer);
			glGenTextures(1, &tileBufferTexture);
			lastTileBufferBytes = bufferSize;
		}

		std::vector<uint32_t> tileData(static_cast<size_t>(tileCount) * TILE_STRIDE, 0);

		int sdfCount = 0;
		for (int slot = 0; slot < static_cast<int>(SDFUBO::SDF_MESH_COUNT); ++slot) {
			const size_t meshBase = slot * SDFUBO::MESH_STRIDE;
			if (*reinterpret_cast<const int*>(sdfCmd->uboData.data() + meshBase + SDFUBO::MESH_SIZE) <= 0) {
				break;
			}
			sdfCount = slot + 1;
		}

		for (int ty = 0; ty < tilesY; ++ty) {
			for (int tx = 0; tx < tilesX; ++tx) {
				const int tileIdx = ty * tilesX + tx;
				std::vector<int> indices;
				indices.reserve(MAX_SDF_PER_TILE);

				const float tileMinX = static_cast<float>(tx * TILE_SIZE);
				const float tileMaxX = static_cast<float>((tx + 1) * TILE_SIZE);
				const float tileMinY = static_cast<float>(ty * TILE_SIZE);
				const float tileMaxY = static_cast<float>((ty + 1) * TILE_SIZE);

				for (int slot = 0; slot < sdfCount; ++slot) {
					const size_t meshBase = slot * SDFUBO::MESH_STRIDE;
					Vec3 center;
					std::memcpy(&center, sdfCmd->uboData.data() + meshBase + SDFUBO::MESH_BOUND_CENTER, 12);
					float radius = *reinterpret_cast<const float*>(sdfCmd->uboData.data() + meshBase + SDFUBO::MESH_BOUND_RADIUS);

					float sx, sy, clipW;
					ProjectToScreen(center, viewProj, screenW, screenH, sx, sy, clipW);
					if (clipW < 0.01f) {
						continue;
					}
					const float screenRadius = radius * static_cast<float>(screenH) / std::max(clipW, 0.001f);
					const float margin = 1.5f;
					const float sdfMinX = sx - screenRadius * margin;
					const float sdfMaxX = sx + screenRadius * margin;
					const float sdfMinY = sy - screenRadius * margin;
					const float sdfMaxY = sy + screenRadius * margin;

					if (sdfMinX < tileMaxX && sdfMaxX > tileMinX && sdfMinY < tileMaxY && sdfMaxY > tileMinY) {
						indices.push_back(slot);
						if (static_cast<int>(indices.size()) >= MAX_SDF_PER_TILE) {
							break;
						}
					}
				}

				tileData[tileIdx * TILE_STRIDE] = static_cast<uint32_t>(indices.size());
				for (size_t k = 0; k < indices.size(); ++k) {
					tileData[tileIdx * TILE_STRIDE + 1 + k] = static_cast<uint32_t>(indices[k]);
				}
			}
		}

		glBindBuffer(GL_TEXTURE_BUFFER, tileBuffer);
		glBufferData(GL_TEXTURE_BUFFER, bufferSize, tileData.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);

		glBindTexture(GL_TEXTURE_BUFFER, tileBufferTexture);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, tileBuffer);
		glBindTexture(GL_TEXTURE_BUFFER, 0);
	}

	void OpenGLRaymarchingPass::BuildEmptyTileBuffer(int screenW, int screenH) {
		const int tilesX = (screenW + TILE_SIZE - 1) / TILE_SIZE;
		const int tilesY = (screenH + TILE_SIZE - 1) / TILE_SIZE;
		const int tileCount = tilesX * tilesY;
		const int bufferSize = tileCount * TILE_STRIDE * static_cast<int>(sizeof(uint32_t));

		if (tileBuffer == 0 || bufferSize != lastTileBufferBytes) {
			if (tileBuffer != 0) {
				glDeleteBuffers(1, &tileBuffer);
				glDeleteTextures(1, &tileBufferTexture);
			}
			glGenBuffers(1, &tileBuffer);
			glGenTextures(1, &tileBufferTexture);
			lastTileBufferBytes = bufferSize;
		}

		std::vector<uint32_t> tileData(static_cast<size_t>(tileCount) * TILE_STRIDE, 0);
		glBindBuffer(GL_TEXTURE_BUFFER, tileBuffer);
		glBufferData(GL_TEXTURE_BUFFER, bufferSize, tileData.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);

		glBindTexture(GL_TEXTURE_BUFFER, tileBufferTexture);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, tileBuffer);
		glBindTexture(GL_TEXTURE_BUFFER, 0);
	}

	OpenGLRaymarchingPass::OpenGLRaymarchingPass(Framebuffer* inSourceBuffer, Framebuffer* inTargetBuffer) : RenderPass(inSourceBuffer, inTargetBuffer) {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		shaderPass = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseRaymarchingShader"));
		if (!shaderPass) {
			return;
		}

		GLuint programID{ 0 };
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}

		shaderPass->Bind();
		if (texturesIdxs.size() == 4) {
			ShaderBindings::SetUniform<int>(programID, "worldPosIn", texturesIdxs.at(0));
			ShaderBindings::SetUniform<int>(programID, "diffuseIn", texturesIdxs.at(1));
			ShaderBindings::SetUniform<int>(programID, "normalIn", texturesIdxs.at(2));
			ShaderBindings::SetUniform<int>(programID, "uvsIn", texturesIdxs.at(3));
		}
		ShaderBindings::SetUniform<int>(programID, "tileIndicesBuffer", tileBufferTextureIdx);

		quadMesh = dynamic_cast<MeshApi*>(renderer.GetResource("renderQuadNDC"));

		glGenBuffers(1, &sdfUBO);
		if (sdfUBO != 0) {
			glBindBuffer(GL_UNIFORM_BUFFER, sdfUBO);
			glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(SDFUBO::BLOCK_SIZE), nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		GLuint blockIndex = glGetUniformBlockIndex(programID, "SDFBlock");
		if (blockIndex != GL_INVALID_INDEX) {
			glUniformBlockBinding(programID, blockIndex, sdfUBOBindingIndex);
		}
	}

	OpenGLRaymarchingPass::~OpenGLRaymarchingPass() {
		if (tileBuffer != 0) {
			glDeleteBuffers(1, &tileBuffer);
			tileBuffer = 0;
		}
		if (tileBufferTexture != 0) {
			glDeleteTextures(1, &tileBufferTexture);
			tileBufferTexture = 0;
		}
	}

	void OpenGLRaymarchingPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) {
		if (!shaderPass) {
			return;
		}

		shaderPass->Bind();
		sourceBuffer->Bind(FramebufferMode::READING, targetBuffer->GetFBO(), texturesIdxs);
		if (auto* glFbo = dynamic_cast<OpenGLFramebuffer*>(sourceBuffer)) {
			glFbo->BindDepthTexture(depthTextureIdx);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int w = frame.renderWidth > 0 ? frame.renderWidth : Engine::Instance()->GetEffectiveRenderWidth();
		int h = frame.renderHeight > 0 ? frame.renderHeight : Engine::Instance()->GetEffectiveRenderHeight();
		if (w <= 0) {
			w = 1;
		}
		if (h <= 0) {
			h = 1;
		}
		glViewport(0, 0, w, h);

		GLuint programID{ 0 };
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}

		ShaderBindings::SetUniform<Vec2>(programID, "screenSize", Vec2(static_cast<float>(w), static_cast<float>(h)));
		ShaderBindings::SetUniform<int>(programID, "depthIn", depthTextureIdx);
		ShaderBindings::SetUniform<int>(programID, "debugRayMode", 0);

		SDFBufferRenderCmd* sdfBufferCmd = nullptr;
		Mat4 viewProj;
		for (const RenderBatch& renderBatch : frame.renderBatches) {
			auto* cameraRenderCmd = renderBatch.GetCameraRenderCmd();
			if (cameraRenderCmd) {
				viewProj = cameraRenderCmd->projectionMatrix * cameraRenderCmd->viewMatrix;
				ShaderBindings::SetUniform<Mat4>(programID, "ViewMatrix", cameraRenderCmd->viewMatrix);
				ShaderBindings::SetUniform<Mat4>(programID, "ProjectionMatrix", cameraRenderCmd->projectionMatrix);
				ShaderBindings::SetUniform<Vec3>(programID, "cameraPos", cameraRenderCmd->cameraPos);
				ShaderBindings::SetUniform<Mat4>(programID, "cameraToWorld", cameraRenderCmd->cameraToWorld);
			}
			for (const auto& renderCmd : renderBatch.GetSDFRenderCmds()) {
				if (auto* sdf = dynamic_cast<SDFBufferRenderCmd*>(renderCmd.get())) {
					sdfBufferCmd = sdf;
					if (sdfUBO != 0 && !sdf->uboData.empty()) {
						glBindBuffer(GL_UNIFORM_BUFFER, sdfUBO);
						glBufferSubData(GL_UNIFORM_BUFFER, 0,
							static_cast<GLsizeiptr>(sdf->uboData.size()),
							sdf->uboData.data());
						glBindBufferBase(GL_UNIFORM_BUFFER, sdfUBOBindingIndex, sdfUBO);
						glBindBuffer(GL_UNIFORM_BUFFER, 0);
					}
					break;
				}
			}
		}

		const int tilesX = (w + TILE_SIZE - 1) / TILE_SIZE;
		const int tilesY = (h + TILE_SIZE - 1) / TILE_SIZE;
		ShaderBindings::SetUniform<int>(programID, "tilesX", tilesX);
		ShaderBindings::SetUniform<int>(programID, "tilesY", tilesY);
		ShaderBindings::SetUniform<int>(programID, "tileSize", TILE_SIZE);

		if (sdfBufferCmd && !sdfBufferCmd->uboData.empty()) {
			BuildTileBuffer(sdfBufferCmd, viewProj, w, h);
		} else {
			BuildEmptyTileBuffer(w, h);
		}
		if (tileBufferTexture != 0) {
			glActiveTexture(GL_TEXTURE0 + tileBufferTextureIdx);
			glBindTexture(GL_TEXTURE_BUFFER, tileBufferTexture);
		}

		glDisable(GL_CULL_FACE);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
	}

}
