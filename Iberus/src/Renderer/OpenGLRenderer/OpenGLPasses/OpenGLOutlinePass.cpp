#include "Enginepch.h"
#include "OpenGLOutlinePass.h"

#include "Engine.h"
#include "Framebuffer.h"
#include "RenderSettings.h"
#include "RenderCmd.h"
#include "ShaderBindings.h"
#include "OpenGLShader.h"

#include "ShaderApi.h"
#include "TextureApi.h"
#include "MeshApi.h"

namespace Iberus {

	OpenGLOutlinePass::OpenGLOutlinePass() {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		shaderPass = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseOutlineShader"));
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
	}

	void OpenGLOutlinePass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame, Framebuffer* source, Framebuffer* target) {
		if (!shaderPass || !source || !target) {
			return;
		}

		const PostProcessSettings& settings = Engine::Instance()->GetPostProcessSettings();
		bool applyOutline = settings.enableOutlineHighlight;

		float shadowStrength = applyOutline ? settings.outlineShadowStrength : 0.0f;
		float highlightStrength = applyOutline ? settings.outlineHighlightStrength : 0.0f;

		shaderPass->Bind();
		source->Bind(FramebufferMode::READING, target->GetFBO(), texturesIdxs);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLuint programID{ 0 };
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}

		int effW = frame.renderWidth > 0 ? frame.renderWidth : Engine::Instance()->GetEffectiveRenderWidth();
		int effH = frame.renderHeight > 0 ? frame.renderHeight : Engine::Instance()->GetEffectiveRenderHeight();
		ShaderBindings::SetUniform<Vec2>(programID, "screenSize", Vec2(static_cast<float>(effW), static_cast<float>(effH)));
		ShaderBindings::SetUniform<float>(programID, "shadowStrength", shadowStrength);
		ShaderBindings::SetUniform<float>(programID, "highlightStrength", highlightStrength);
		ShaderBindings::SetUniform<Vec3>(programID, "shadowColor", settings.outlineShadowColor);
		ShaderBindings::SetUniform<Vec3>(programID, "highlightColor", settings.outlineHighlightColor);

		Vec3 cameraPos{ 0.0f, 0.0f, 0.0f };
		for (const RenderBatch& renderBatch : frame.renderBatches) {
			auto* cameraRenderCmd = renderBatch.GetCameraRenderCmd();
			if (cameraRenderCmd) {
				cameraPos = cameraRenderCmd->cameraPos;
				break;
			}
		}
		ShaderBindings::SetUniform<Vec3>(programID, "cameraPos", cameraPos);

		glDisable(GL_CULL_FACE);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
	}

}
