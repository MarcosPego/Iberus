#include "Enginepch.h"
#include "OpenGLRaymarchingPass.h"

#include "Engine.h"
#include "Framebuffer.h"
#include "Matrix.h"
#include "ShaderBindings.h"
#include "OpenGLShader.h"
#include "OpenGLComputeShader.h"
#include "OpenGLFramebuffer.h"
#include "RenderCmd.h"

#include "ShaderApi.h"
#include "ComputeShaderApi.h"
#include "TextureApi.h"
#include "MeshApi.h"

#include <algorithm>
#include <cstring>
#include <vector>

using namespace Math;

namespace Iberus {

	static int sdfDebugFrameCounter = 0;
	static constexpr int SDF_DEBUG_LOG_INTERVAL = 120;  // Log every ~2 sec at 60fps

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

		const int creatureCount = sdfCmd->uboData.size() >= 8
			? *reinterpret_cast<const int*>(sdfCmd->uboData.data() + SDFSSBO::GLOBAL_CREATURE_COUNT)
			: 0;

		for (int ty = 0; ty < tilesY; ++ty) {
			for (int tx = 0; tx < tilesX; ++tx) {
				const int tileIdx = ty * tilesX + tx;
				std::vector<int> indices;
				indices.reserve(MAX_SDF_PER_TILE);

				const float tileMinX = static_cast<float>(tx * TILE_SIZE);
				const float tileMaxX = static_cast<float>((tx + 1) * TILE_SIZE);
				const float tileMinY = static_cast<float>(ty * TILE_SIZE);
				const float tileMaxY = static_cast<float>((ty + 1) * TILE_SIZE);

				for (int slot = 0; slot < creatureCount; ++slot) {
					const size_t headerBase = SDFSSBO::GLOBAL_HEADER_SIZE + slot * SDFSSBO::CREATURE_HEADER_STRIDE;
					if (headerBase + SDFSSBO::CREATURE_HEADER_STRIDE > sdfCmd->uboData.size()) {
						break;
					}
					Vec3 center;
					std::memcpy(&center, sdfCmd->uboData.data() + headerBase + SDFSSBO::CREATURE_HEADER_BOUND_CENTER, 12);
					float radius = *reinterpret_cast<const float*>(sdfCmd->uboData.data() + headerBase + SDFSSBO::CREATURE_HEADER_BOUND_RADIUS);

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

		int tilesWithSDF = 0;
		for (int i = 0; i < tileCount; ++i) {
			if (tileData[i * TILE_STRIDE] > 0) {
				tilesWithSDF++;
			}
		}
		if (++sdfDebugFrameCounter % SDF_DEBUG_LOG_INTERVAL == 0) {
			IB_CORE_INFO("[SDF] Tile path=CPU creatureCount={} tilesWithSDF={}/{} tilesX={} tilesY={}",
				creatureCount, tilesWithSDF, tileCount, tilesX, tilesY);
		}
	}

	static void EnsureCoarseFBO(unsigned int& fbo, unsigned int& tex, int& outW, int& outH, int fullW, int fullH, int scale, bool forceFullRes = false) {
		const int cw = forceFullRes ? fullW : std::max(1, (fullW + scale - 1) / scale);
		const int ch = forceFullRes ? fullH : std::max(1, (fullH + scale - 1) / scale);
		if (fbo == 0 || outW != cw || outH != ch) {
			if (fbo != 0) {
				glDeleteFramebuffers(1, &fbo);
				glDeleteTextures(1, &tex);
			}
			glGenFramebuffers(1, &fbo);
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, cw, ch, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				IB_CORE_ERROR("[SDF] Coarse FBO incomplete: 0x{:X} (fbo={} tex={} {}x{})", status, fbo, tex, cw, ch);
			} else if (sdfDebugFrameCounter % SDF_DEBUG_LOG_INTERVAL == 0) {
				IB_CORE_INFO("[SDF] Coarse FBO created: fbo={} tex={} {}x{} (from {}x{})", fbo, tex, cw, ch, fullW, fullH);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			outW = cw;
			outH = ch;
		}
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

	OpenGLRaymarchingPass::OpenGLRaymarchingPass() {
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
		coarseShaderPass = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseRaymarchingCoarseShader"));
		coarseDebugBlitPass = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/coarseDebugBlit"));
		coarseDirectTestPass = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/coarseDirectTest"));
		tileBufferComputePass = dynamic_cast<ComputeShaderApi*>(renderer.GetResource("assets/shaders/sdfTileBufferCompute"));

		if (!coarseShaderPass) {
			IB_CORE_WARN("[SDF] Coarse shader not loaded - baseRaymarchingCoarseShader missing from Engine::SetupDeferredRenderer");
		}
		if (!coarseDebugBlitPass) {
			IB_CORE_WARN("[SDF] Coarse debug blit shader not loaded");
		}
		if (!coarseDirectTestPass) {
			IB_CORE_WARN("[SDF] Coarse direct test shader not loaded");
		}

		glGenBuffers(1, &sdfSSBO);
		if (sdfSSBO != 0) {
			sdfSSBOCapacity = 0;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, sdfSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(SDFSSBO::GLOBAL_HEADER_SIZE), nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		GLuint blockIndex = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "SDFBlock");
		if (blockIndex != GL_INVALID_INDEX) {
			glShaderStorageBlockBinding(programID, blockIndex, sdfSSBOBindingIndex);
		}
		if (coarseShaderPass) {
			GLuint coarseBlockIdx = 0;
			if (auto* coarseGL = dynamic_cast<OpenGLShader*>(coarseShaderPass); coarseGL) {
				coarseGL->Bind();
				coarseBlockIdx = glGetProgramResourceIndex(coarseGL->GetProgramID(), GL_SHADER_STORAGE_BLOCK, "SDFBlock");
				if (coarseBlockIdx != GL_INVALID_INDEX) {
					glShaderStorageBlockBinding(coarseGL->GetProgramID(), coarseBlockIdx, sdfSSBOBindingIndex);
				}
				coarseGL->Unbind();
			}
		}
		shaderPass->Bind();
	}

	void OpenGLRaymarchingPass::BuildTileBufferGPU(const Math::Mat4& viewProj, int screenW, int screenH) {
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

		glBindBuffer(GL_TEXTURE_BUFFER, tileBuffer);
		glBufferData(GL_TEXTURE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);

		if (!tileBufferComputePass || tileBufferComputePass->GetProgramID() == 0) {
			BuildEmptyTileBuffer(screenW, screenH);
			return;
		}
		tileBufferComputePass->Bind();
		unsigned int progID = tileBufferComputePass->GetProgramID();
		ShaderBindings::SetUniform<Mat4>(progID, "viewProj", viewProj);
		ShaderBindings::SetUniform<int>(progID, "screenW", screenW);
		ShaderBindings::SetUniform<int>(progID, "screenH", screenH);
		ShaderBindings::SetUniform<int>(progID, "tileSize", TILE_SIZE);
		ShaderBindings::SetUniform<int>(progID, "tilesX", tilesX);
		ShaderBindings::SetUniform<int>(progID, "tilesY", tilesY);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sdfSSBOBindingIndex, sdfSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, tileBufferSSBOBindingIndex, tileBuffer);

		glDispatchCompute(
			static_cast<GLuint>((tilesX + 7) / 8),
			static_cast<GLuint>((tilesY + 7) / 8),
			1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sdfSSBOBindingIndex, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, tileBufferSSBOBindingIndex, 0);

		glBindTexture(GL_TEXTURE_BUFFER, tileBufferTexture);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, tileBuffer);
		glBindTexture(GL_TEXTURE_BUFFER, 0);

		if (++sdfDebugFrameCounter % SDF_DEBUG_LOG_INTERVAL == 0) {
			IB_CORE_INFO("[SDF] Tile path=GPU tilesX={} tilesY={} screenW={} screenH={}", tilesX, tilesY, screenW, screenH);
		}
	}

	OpenGLRaymarchingPass::~OpenGLRaymarchingPass() {
		if (coarseFBO != 0) {
			glDeleteFramebuffers(1, &coarseFBO);
			coarseFBO = 0;
		}
		if (coarseTexture != 0) {
			glDeleteTextures(1, &coarseTexture);
			coarseTexture = 0;
		}
		if (sdfSSBO != 0) {
			glDeleteBuffers(1, &sdfSSBO);
			sdfSSBO = 0;
		}
		if (tileBuffer != 0) {
			glDeleteBuffers(1, &tileBuffer);
			tileBuffer = 0;
		}
		if (tileBufferTexture != 0) {
			glDeleteTextures(1, &tileBufferTexture);
			tileBufferTexture = 0;
		}
	}

	// --- ExecutePass pipeline stages (refactored for debuggability) ---

	void OpenGLRaymarchingPass::SetupCameraAndSDFData(Frame& frame, int w, int h, GLuint programID,
		SDFBufferRenderCmd*& outSdfCmd, Math::Mat4& outViewProj, Math::Mat4& outView, Math::Mat4& outProj) {
		outSdfCmd = nullptr;
		for (const RenderBatch& renderBatch : frame.renderBatches) {
			auto* cameraRenderCmd = renderBatch.GetCameraRenderCmd();
			if (cameraRenderCmd) {
				outViewProj = cameraRenderCmd->projectionMatrix * cameraRenderCmd->viewMatrix;
				outView = cameraRenderCmd->viewMatrix;
				outProj = cameraRenderCmd->projectionMatrix;
				ShaderBindings::SetUniform<Mat4>(programID, "ViewMatrix", cameraRenderCmd->viewMatrix);
				ShaderBindings::SetUniform<Mat4>(programID, "ProjectionMatrix", cameraRenderCmd->projectionMatrix);
				ShaderBindings::SetUniform<Vec3>(programID, "cameraPos", cameraRenderCmd->cameraPos);
				ShaderBindings::SetUniform<Mat4>(programID, "cameraToWorld", cameraRenderCmd->cameraToWorld);
				const Mat4& proj = cameraRenderCmd->projectionMatrix;
				float tanHalfFovY = (std::abs(proj.data[5]) > 1e-6f) ? (1.0f / proj.data[5]) : 0.5f;
				float pixelConeWidth = (2.0f * tanHalfFovY) / static_cast<float>(h);
				float aperture = pixelConeWidth * 0.5f;
				float c = std::sqrt(aperture * aperture + 1.0f);
				float coneTraceA = (std::abs(c - aperture) > 1e-6f) ? (c / (c - aperture)) : 1.0f;
				ShaderBindings::SetUniform<float>(programID, "pixelConeWidth", pixelConeWidth);
				ShaderBindings::SetUniform<float>(programID, "coneTraceA", coneTraceA);
			}
			for (const auto& renderCmd : renderBatch.GetSDFRenderCmds()) {
				if (auto* sdf = dynamic_cast<SDFBufferRenderCmd*>(renderCmd.get())) {
					outSdfCmd = sdf;
					if (sdfSSBO != 0) {
						const size_t uploadSize = sdf->uboData.empty() ? SDFSSBO::GLOBAL_HEADER_SIZE : sdf->uboData.size();
						if (uploadSize > sdfSSBOCapacity) {
							sdfSSBOCapacity = std::max(uploadSize, sdfSSBOCapacity * 2);
							sdfSSBOCapacity = std::max(sdfSSBOCapacity, static_cast<size_t>(SDFSSBO::GLOBAL_HEADER_SIZE));
							glBindBuffer(GL_SHADER_STORAGE_BUFFER, sdfSSBO);
							glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(sdfSSBOCapacity), nullptr, GL_DYNAMIC_DRAW);
						}
						glBindBuffer(GL_SHADER_STORAGE_BUFFER, sdfSSBO);
						if (sdf->uboData.empty()) {
							std::vector<uint8_t> emptyHeader(SDFSSBO::GLOBAL_HEADER_SIZE, 0);
							glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, static_cast<GLsizeiptr>(emptyHeader.size()), emptyHeader.data());
						} else {
							glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
								static_cast<GLsizeiptr>(sdf->uboData.size()),
								sdf->uboData.data());
						}
						glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sdfSSBOBindingIndex, sdfSSBO);
						glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
					}
					break;
				}
			}
		}
	}

	void OpenGLRaymarchingPass::SetupTileBuffer(const SDFBufferRenderCmd* sdfCmd, const Math::Mat4& viewProj, int w, int h) {
		const int tilesX = (w + TILE_SIZE - 1) / TILE_SIZE;
		const int tilesY = (h + TILE_SIZE - 1) / TILE_SIZE;
		GLuint programID = 0;
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}
		ShaderBindings::SetUniform<int>(programID, "tilesX", tilesX);
		ShaderBindings::SetUniform<int>(programID, "tilesY", tilesY);
		ShaderBindings::SetUniform<int>(programID, "tileSize", TILE_SIZE);

		if (sdfCmd && !sdfCmd->uboData.empty()) {
			const bool useGPU = !Engine::Instance()->GetPostProcessSettings().sdfForceCPUTiles
				&& tileBufferComputePass && tileBufferComputePass->GetProgramID() != 0;
			if (useGPU) {
				BuildTileBufferGPU(viewProj, w, h);
			} else {
				BuildTileBuffer(sdfCmd, viewProj, w, h);
			}
		} else {
			BuildEmptyTileBuffer(w, h);
			if (++sdfDebugFrameCounter % SDF_DEBUG_LOG_INTERVAL == 0) {
				IB_CORE_INFO("[SDF] No SDF data - empty tile buffer (no creatures or no SDFBufferRenderCmd)");
			}
		}
		if (tileBufferTexture != 0) {
			glActiveTexture(GL_TEXTURE0 + tileBufferTextureIdx);
			glBindTexture(GL_TEXTURE_BUFFER, tileBufferTexture);
		}
	}

	bool OpenGLRaymarchingPass::RunCoarsePass(int w, int h, const SDFBufferRenderCmd* sdfCmd,
		const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix) {
		const int coarseDebugMode = Engine::Instance()->GetPostProcessSettings().sdfCoarseDebugMode;
		const bool wantCoarseForDebug = (Engine::Instance()->GetPostProcessSettings().sdfDebugMode == 10 ||
			Engine::Instance()->GetPostProcessSettings().sdfDebugMode == 11 ||
			Engine::Instance()->GetPostProcessSettings().sdfDebugMode == 12);
		const bool useFullResForPipelineTest = wantCoarseForDebug && (coarseDebugMode == 5);
		EnsureCoarseFBO(coarseFBO, coarseTexture, coarseWidth, coarseHeight, w, h, COARSE_SCALE, useFullResForPipelineTest);
		if (coarseFBO == 0) {
			return false;
		}
		coarseShaderPass->Bind();
		GLuint coarseProgramID = 0;
		if (auto* coarseGL = dynamic_cast<OpenGLShader*>(coarseShaderPass); coarseGL) {
			coarseProgramID = coarseGL->GetProgramID();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, coarseFBO);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glDisable(GL_SCISSOR_TEST);
		glViewport(0, 0, coarseWidth, coarseHeight);
		ShaderBindings::SetUniform<Vec2>(coarseProgramID, "screenSize", Vec2(static_cast<float>(w), static_cast<float>(h)));
		ShaderBindings::SetUniform<int>(coarseProgramID, "tilesX", (w + TILE_SIZE - 1) / TILE_SIZE);
		ShaderBindings::SetUniform<int>(coarseProgramID, "tilesY", (h + TILE_SIZE - 1) / TILE_SIZE);
		ShaderBindings::SetUniform<int>(coarseProgramID, "tileSize", TILE_SIZE);
		ShaderBindings::SetUniform<int>(coarseProgramID, "tileIndicesBuffer", tileBufferTextureIdx);
		ShaderBindings::SetUniform<int>(coarseProgramID, "coarseScale", COARSE_SCALE);
		ShaderBindings::SetUniform<int>(coarseProgramID, "coarseDebugMode", wantCoarseForDebug ? coarseDebugMode : 0);
		ShaderBindings::SetUniform<int>(coarseProgramID, "outputToTarget", 0);
		ShaderBindings::SetUniform<Mat4>(coarseProgramID, "ViewMatrix", viewMatrix);
		ShaderBindings::SetUniform<Mat4>(coarseProgramID, "ProjectionMatrix", projMatrix);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sdfSSBOBindingIndex, sdfSSBO);
		glActiveTexture(GL_TEXTURE0 + tileBufferTextureIdx);
		glBindTexture(GL_TEXTURE_BUFFER, tileBufferTexture);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
		glFinish();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	void OpenGLRaymarchingPass::ExecuteCoarseToTarget(int w, int h, Framebuffer* source, Framebuffer* target,
		const Math::Mat4& viewMatrix, const Math::Mat4& projMatrix) {
		// Coarse shader draws directly to target FBO (4 attachments). Needs geometry textures for pass-through.
		source->Bind(FramebufferMode::READING, target->GetFBO(), texturesIdxs);
		coarseShaderPass->Bind();
		GLuint coarseProgID = 0;
		if (auto* cgl = dynamic_cast<OpenGLShader*>(coarseShaderPass); cgl) {
			coarseProgID = cgl->GetProgramID();
		}
		glViewport(0, 0, w, h);
		ShaderBindings::SetUniform<Vec2>(coarseProgID, "screenSize", Vec2(static_cast<float>(w), static_cast<float>(h)));
		ShaderBindings::SetUniform<int>(coarseProgID, "tilesX", (w + TILE_SIZE - 1) / TILE_SIZE);
		ShaderBindings::SetUniform<int>(coarseProgID, "tilesY", (h + TILE_SIZE - 1) / TILE_SIZE);
		ShaderBindings::SetUniform<int>(coarseProgID, "tileSize", TILE_SIZE);
		ShaderBindings::SetUniform<int>(coarseProgID, "tileIndicesBuffer", tileBufferTextureIdx);
		ShaderBindings::SetUniform<int>(coarseProgID, "coarseScale", COARSE_SCALE);
		ShaderBindings::SetUniform<int>(coarseProgID, "coarseDebugMode", Engine::Instance()->GetPostProcessSettings().sdfCoarseDebugMode);
		ShaderBindings::SetUniform<int>(coarseProgID, "outputToTarget", 1);
		ShaderBindings::SetUniform<int>(coarseProgID, "worldPosIn", texturesIdxs.at(0));
		ShaderBindings::SetUniform<int>(coarseProgID, "normalIn", texturesIdxs.at(2));
		ShaderBindings::SetUniform<int>(coarseProgID, "uvsIn", texturesIdxs.at(3));
		ShaderBindings::SetUniform<Mat4>(coarseProgID, "ViewMatrix", viewMatrix);
		ShaderBindings::SetUniform<Mat4>(coarseProgID, "ProjectionMatrix", projMatrix);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sdfSSBOBindingIndex, sdfSSBO);
		glActiveTexture(GL_TEXTURE0 + tileBufferTextureIdx);
		glBindTexture(GL_TEXTURE_BUFFER, tileBufferTexture);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
	}

	void OpenGLRaymarchingPass::ExecuteDirectTest(int w, int h, Framebuffer* source, Framebuffer* target) {
		// Simple gradient shader to target - no coarse texture. Verifies target FBO draw works.
		coarseDirectTestPass->Bind();
		source->Bind(FramebufferMode::READING, target->GetFBO(), texturesIdxs);
		glViewport(0, 0, w, h);
		GLuint directProgID = 0;
		if (auto* directGL = dynamic_cast<OpenGLShader*>(coarseDirectTestPass); directGL) {
			directProgID = directGL->GetProgramID();
		}
		ShaderBindings::SetUniform<Vec2>(directProgID, "screenSize", Vec2(static_cast<float>(w), static_cast<float>(h)));
		ShaderBindings::SetUniform<int>(directProgID, "worldPosIn", texturesIdxs.at(0));
		ShaderBindings::SetUniform<int>(directProgID, "normalIn", texturesIdxs.at(2));
		ShaderBindings::SetUniform<int>(directProgID, "uvsIn", texturesIdxs.at(3));
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
	}

	void OpenGLRaymarchingPass::ExecuteCoarseFramebufferBlit(int w, int h, Framebuffer* target) {
		// Raw glBlitFramebuffer: coarse FBO -> target. Do not change glDrawBuffers - target has 4 attachments.
		glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, coarseFBO);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->GetFBO());
		glBlitFramebuffer(0, 0, coarseWidth, coarseHeight, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLRaymarchingPass::ExecuteBlit(int w, int h, Framebuffer* source, Framebuffer* target) {
		// Blit shader samples coarseTexture and writes to target. Ensure coarse FBO write is visible.
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
		coarseDebugBlitPass->Bind();
		source->Bind(FramebufferMode::READING, target->GetFBO(), texturesIdxs);
		glViewport(0, 0, w, h);
		GLuint blitProgramID = 0;
		if (auto* blitGL = dynamic_cast<OpenGLShader*>(coarseDebugBlitPass); blitGL) {
			blitProgramID = blitGL->GetProgramID();
		}
		ShaderBindings::SetUniform<Vec2>(blitProgramID, "screenSize", Vec2(static_cast<float>(w), static_cast<float>(h)));
		ShaderBindings::SetUniform<int>(blitProgramID, "coarseScale", COARSE_SCALE);
		ShaderBindings::SetUniform<int>(blitProgramID, "coarseTex", coarseMaskTextureIdx);
		ShaderBindings::SetUniform<int>(blitProgramID, "worldPosIn", texturesIdxs.at(0));
		ShaderBindings::SetUniform<int>(blitProgramID, "normalIn", texturesIdxs.at(2));
		ShaderBindings::SetUniform<int>(blitProgramID, "uvsIn", texturesIdxs.at(3));
		glActiveTexture(GL_TEXTURE0 + coarseMaskTextureIdx);
		glBindTexture(GL_TEXTURE_2D, coarseTexture);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
	}

	void OpenGLRaymarchingPass::ExecuteMainRaymarch(int w, int h, Framebuffer* source, Framebuffer* target, GLuint programID,
		bool useCoarseMask, bool showCoarseDebug) {
		shaderPass->Bind();
		source->Bind(FramebufferMode::READING, target->GetFBO(), texturesIdxs);
		if (auto* glFbo = dynamic_cast<OpenGLFramebuffer*>(source)) {
			glFbo->BindDepthTexture(depthTextureIdx);
		}
		glViewport(0, 0, w, h);
		const int coarseDebugMode = Engine::Instance()->GetPostProcessSettings().sdfCoarseDebugMode;
		ShaderBindings::SetUniform<int>(programID, "useCoarsePass", (useCoarseMask || showCoarseDebug) ? 1 : 0);
		ShaderBindings::SetUniform<int>(programID, "coarseMask", coarseMaskTextureIdx);
		ShaderBindings::SetUniform<int>(programID, "coarseDebugConstant", (coarseDebugMode == 3) ? 1 : 0);
		ShaderBindings::SetUniform<Vec2>(programID, "coarseTexSize", Vec2(static_cast<float>(coarseWidth), static_cast<float>(coarseHeight)));
		if ((useCoarseMask || showCoarseDebug) && coarseTexture != 0) {
			glActiveTexture(GL_TEXTURE0 + coarseMaskTextureIdx);
			glBindTexture(GL_TEXTURE_2D, coarseTexture);
		}
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
	}

	void OpenGLRaymarchingPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame, Framebuffer* source, Framebuffer* target) {
		if (!shaderPass || !source || !target) {
			return;
		}

		int w = frame.renderWidth > 0 ? frame.renderWidth : Engine::Instance()->GetEffectiveRenderWidth();
		int h = frame.renderHeight > 0 ? frame.renderHeight : Engine::Instance()->GetEffectiveRenderHeight();
		if (w <= 0) {
			w = 1;
		}
		if (h <= 0) {
			h = 1;
		}

		shaderPass->Bind();
		source->Bind(FramebufferMode::READING, target->GetFBO(), texturesIdxs);
		if (auto* glFbo = dynamic_cast<OpenGLFramebuffer*>(source)) {
			glFbo->BindDepthTexture(depthTextureIdx);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, w, h);

		GLuint programID = 0;
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}
		ShaderBindings::SetUniform<Vec2>(programID, "screenSize", Vec2(static_cast<float>(w), static_cast<float>(h)));
		ShaderBindings::SetUniform<int>(programID, "depthIn", depthTextureIdx);
		ShaderBindings::SetUniform<int>(programID, "debugRayMode", Engine::Instance()->GetPostProcessSettings().sdfDebugMode);
		ShaderBindings::SetUniform<float>(programID, "pixelConeWidth", 0.01f);
		ShaderBindings::SetUniform<float>(programID, "coneTraceA", 1.0f);

		SDFBufferRenderCmd* sdfBufferCmd = nullptr;
		Mat4 viewProj;
		Mat4 viewMatrix;
		Mat4 projMatrix;
		SetupCameraAndSDFData(frame, w, h, programID, sdfBufferCmd, viewProj, viewMatrix, projMatrix);
		SetupTileBuffer(sdfBufferCmd, viewProj, w, h);

		const bool wantCoarseForCulling = Engine::Instance()->GetPostProcessSettings().sdfUseCoarsePass;
		const int sdfDebugMode = Engine::Instance()->GetPostProcessSettings().sdfDebugMode;
		const bool wantCoarseForDebug = (sdfDebugMode == 10 || sdfDebugMode == 11 || sdfDebugMode == 12);
		const bool showCoarseDebug = (sdfDebugMode == 10);
		const int coarseDebugMode = Engine::Instance()->GetPostProcessSettings().sdfCoarseDebugMode;

		const bool hasSdfData = sdfBufferCmd && !sdfBufferCmd->uboData.empty();
		const bool runCoarseCondition = (wantCoarseForCulling || wantCoarseForDebug) && coarseShaderPass;

		bool useCoarseMask = false;
		if (runCoarseCondition) {
			if (hasSdfData) {
				useCoarseMask = RunCoarsePass(w, h, sdfBufferCmd, viewMatrix, projMatrix);
			} else if (showCoarseDebug || sdfDebugMode == 12) {
				// No SDF data but debugging coarse: run anyway to create FBO (Pipeline test outputs white)
				useCoarseMask = RunCoarsePass(w, h, nullptr, viewMatrix, projMatrix);
			}
		}

		// Output path selection: which shader writes to target?
		// CoarseToTarget: coarse shader draws directly to target (bypasses coarse FBO).
		// Mode 10: showCoarseDebug. Mode 12 + Pipeline test: use CoarseToTarget (coarse FBO path is unreliable).
		const bool useCoarseToTargetPath = (showCoarseDebug || (sdfDebugMode == 12 && coarseDebugMode == 5))
			&& (coarseDebugMode == 0 || coarseDebugMode == 1 || coarseDebugMode == 2 || coarseDebugMode == 5)
			&& coarseShaderPass;
		const bool useDirectTestPath = showCoarseDebug && coarseDebugMode == 4 && coarseDirectTestPass;
		const bool useFramebufferBlitPath = false;
		// Blit shader: samples coarse texture and writes to target.
		const bool useBlitPath = false;

		if (sdfDebugFrameCounter % SDF_DEBUG_LOG_INTERVAL == 0 && (showCoarseDebug || wantCoarseForCulling)) {
			const char* pathName = useCoarseToTargetPath ? "CoarseToTarget" : useDirectTestPath ? "DirectTest"
				: useFramebufferBlitPath ? "FramebufferBlit" : useBlitPath ? "Blit" : "Main";
			IB_CORE_INFO("[SDF Coarse] path={} coarseDebugMode={} ranCoarse={} coarseFBO={}x{} tex={} (hasSdf={} hasCoarseShader={})",
				pathName, coarseDebugMode, useCoarseMask ? 1 : 0, coarseWidth, coarseHeight, coarseTexture,
				hasSdfData ? 1 : 0, coarseShaderPass ? 1 : 0);
		}

		if (useCoarseToTargetPath) {
			ExecuteCoarseToTarget(w, h, source, target, viewMatrix, projMatrix);
		} else if (useDirectTestPath) {
			ExecuteDirectTest(w, h, source, target);
		} else if (useFramebufferBlitPath) {
			ExecuteCoarseFramebufferBlit(w, h, target);
		} else if (useBlitPath) {
			ExecuteBlit(w, h, source, target);
		} else {
			if (useCoarseMask && coarseTexture != 0) {
				glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
			}
			ExecuteMainRaymarch(w, h, source, target, programID, useCoarseMask, showCoarseDebug);
		}
	}

}
