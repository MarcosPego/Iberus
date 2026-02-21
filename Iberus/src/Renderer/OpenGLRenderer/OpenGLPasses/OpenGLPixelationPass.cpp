#include "Enginepch.h"
#include "OpenGLPixelationPass.h"

#include "Engine.h"
#include "Framebuffer.h"
#include "RenderSettings.h"
#include "ShaderBindings.h"
#include "OpenGLShader.h"

#include "ShaderApi.h"
#include "TextureApi.h"
#include "MeshApi.h"

namespace Iberus {

	OpenGLPixelationPass::OpenGLPixelationPass() {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		shaderPass = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/basePixelationShader"));
		if (!shaderPass) {
			return;
		}

		GLuint programID{ 0 };
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}

		shaderPass->Bind();
		if (texturesIdxs.size() >= 1) {
			ShaderBindings::SetUniform<int>(programID, "worldPosIn", texturesIdxs.at(0));
		}

		quadMesh = dynamic_cast<MeshApi*>(renderer.GetResource("renderQuadNDC"));
	}

	void OpenGLPixelationPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame, Framebuffer* source, Framebuffer* target) {
		if (!shaderPass || !source || !target) {
			return;
		}

		const PostProcessSettings& settings = Engine::Instance()->GetPostProcessSettings();
		int pixelCount = 0;
		if (settings.renderStyle == RenderStyle::PixelStyle && settings.enablePixelation) {
			pixelCount = settings.pixelCount > 0 ? settings.pixelCount : 64;
		}

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
		ShaderBindings::SetUniform<int>(programID, "pixelCount", pixelCount);

		glDisable(GL_CULL_FACE);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
	}

}
