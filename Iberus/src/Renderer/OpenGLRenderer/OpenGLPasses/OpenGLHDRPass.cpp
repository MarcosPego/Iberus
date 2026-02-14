#include "Enginepch.h"
#include "OpenGLHDRPass.h"

#include "Engine.h"
#include "Window.h"
#include "Framebuffer.h"
#include "Matrix.h"
#include "ShaderBindings.h"
#include "OpenGLShader.h"

#include "ShaderApi.h"
#include "TextureApi.h"
#include "MeshApi.h"

namespace Iberus {
	OpenGLHDRPass::OpenGLHDRPass(Framebuffer* inSourceBuffer, Framebuffer* inTargetBuffer) : RenderPass(inSourceBuffer, inTargetBuffer) {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		shaderPass = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseHDRShader"));
		if (!shaderPass) {
			return; // TODO(MPP) Logging
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


		ShaderBindings::SetUniform<float>(programID, "exposure", exposure);
		ShaderBindings::SetUniform<float>(programID, "gamma", gamma);
		quadMesh = dynamic_cast<MeshApi*>(renderer.GetResource("renderQuad"));
	}

	void OpenGLHDRPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) {
		if (!shaderPass) {
			return;
		}

		shaderPass->Bind();
		sourceBuffer->Bind(FramebufferMode::READING, targetBuffer->GetFBO(), texturesIdxs);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLuint programID{ 0 };
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}

		auto* engine = Engine::Instance();
		int effW = engine->GetEffectiveRenderWidth();
		int effH = engine->GetEffectiveRenderHeight();
		ShaderBindings::SetUniform<Vec2>(programID, "screenSize", Vec2(static_cast<float>(effW), static_cast<float>(effH)));

		for (const RenderBatch& renderBatch : frame.renderBatches) {
			auto* cameraRenderCmd = renderBatch.GetCameraRenderCmd();
			if (cameraRenderCmd) {
				ShaderBindings::SetUniform<Mat4>(programID, "ViewMatrix", cameraRenderCmd->viewMatrix);
				ShaderBindings::SetUniform<Mat4>(programID, "ProjectionMatrix", cameraRenderCmd->projectionMatrix);
				ShaderBindings::SetUniform<Vec3>(programID, "cameraPos", cameraRenderCmd->cameraPos);
			}
		}

		auto* window = engine->GetCurrentWindow();
		float scaleX = (window && window->GetWidth() > 0) ? static_cast<float>(effW) / static_cast<float>(window->GetWidth()) : 1.0f;
		float scaleY = (window && window->GetHeight() > 0) ? static_cast<float>(effH) / static_cast<float>(window->GetHeight()) : 1.0f;
		auto modelMatrix = MatrixFactory::CreateModelMatrix({ -effW * 0.5f, -effH * 0.5f, 0.0f }, { 0, 0, 0 }, { scaleX, scaleY, 1.0f });
		ShaderBindings::SetUniform<Mat4>(programID, "ModelMatrix", modelMatrix);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
		if (glGetError() != GL_NO_ERROR) {
			//std::cout << "Error in Mesh" << std::endl;
		}
	}
}
