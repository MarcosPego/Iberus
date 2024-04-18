#include "Enginepch.h"
#include "OpenGLDeferredLightPass.h"

#include "Engine.h"
#include "Window.h"
#include "Framebuffer.h"
#include "ShaderBindings.h"
#include "OpenGLShader.h"

#include "ShaderApi.h"
#include "TextureApi.h"
#include "MeshApi.h"

namespace Iberus {

	OpenGLDeferredLightPass::OpenGLDeferredLightPass(Framebuffer* inSourceFBO, Framebuffer* inTargetFBO) : RenderPass(inSourceFBO, inTargetFBO) {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		shaderPass = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseDeferredLightShader"));
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

		quadMesh = dynamic_cast<MeshApi*>(renderer.GetResource("renderQuad"));
	}

	void OpenGLDeferredLightPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) {
		if (!shaderPass) {
			return;
		}

		/*glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);*/

		shaderPass->Bind();
		sourceBuffer->Bind(FramebufferMode::READING, targetBuffer->GetFBO(), texturesIdxs);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLuint programID{ 0 };
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}

		auto* currentWindow = Engine::Instance()->GetCurrentWindow();
		ShaderBindings::SetUniform<Vec2>(programID, "screenSize", Vec2(currentWindow->GetWidth(), currentWindow->GetHeight()));

		for (const RenderBatch& renderBatch : frame.renderBatches) {
			auto* cameraRenderCmd = renderBatch.GetCameraRenderCmd();
			if (cameraRenderCmd) {
				ShaderBindings::SetUniform<Mat4>(programID, "ViewMatrix", cameraRenderCmd->viewMatrix);
				ShaderBindings::SetUniform<Mat4>(programID, "ProjectionMatrix", cameraRenderCmd->projectionMatrix);
				ShaderBindings::SetUniform<Vec3>(programID, "cameraPos", cameraRenderCmd->cameraPos);
			}
		}

		static const auto modelMatrix = MatrixFactory::CreateModelMatrix({ -currentWindow->GetWidth() * 0.5f, -currentWindow->GetHeight() * 0.5f, 0.0 }, { 0,0,0 }, { 1,1,1 });

		// RenderQuad 
		ShaderBindings::SetUniform<Mat4>(programID, "ModelMatrix", modelMatrix);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
		if (glGetError() != GL_NO_ERROR) {
			//std::cout << "Error in Mesh" << std::endl;
		}

	}
}

