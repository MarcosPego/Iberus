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

namespace Iberus {

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

		for (const RenderBatch& renderBatch : frame.renderBatches) {
			auto* cameraRenderCmd = renderBatch.GetCameraRenderCmd();
			if (cameraRenderCmd) {
				ShaderBindings::SetUniform<Mat4>(programID, "ViewMatrix", cameraRenderCmd->viewMatrix);
				ShaderBindings::SetUniform<Mat4>(programID, "ProjectionMatrix", cameraRenderCmd->projectionMatrix);
				ShaderBindings::SetUniform<Vec3>(programID, "cameraPos", cameraRenderCmd->cameraPos);
				ShaderBindings::SetUniform<Mat4>(programID, "cameraToWorld", cameraRenderCmd->cameraToWorld);
			}
			for (const auto& renderCmd : renderBatch.GetSDFRenderCmds()) {
				if (auto* sdfBufferCmd = dynamic_cast<SDFBufferRenderCmd*>(renderCmd.get())) {
					if (sdfUBO != 0 && !sdfBufferCmd->uboData.empty()) {
						glBindBuffer(GL_UNIFORM_BUFFER, sdfUBO);
						glBufferSubData(GL_UNIFORM_BUFFER, 0,
							static_cast<GLsizeiptr>(sdfBufferCmd->uboData.size()),
							sdfBufferCmd->uboData.data());
						glBindBufferBase(GL_UNIFORM_BUFFER, sdfUBOBindingIndex, sdfUBO);
						glBindBuffer(GL_UNIFORM_BUFFER, 0);
					}
					break;
				}
			}
		}

		glDisable(GL_CULL_FACE);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
	}

}
