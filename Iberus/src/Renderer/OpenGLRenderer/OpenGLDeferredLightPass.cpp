#include "Enginepch.h"
#include "OpenGLDeferredLightPass.h"

#include "Engine.h"
#include "Window.h"
#include "Framebuffer.h"
#include "ShaderBindings.h"
#include "OpenGLShader.h"

namespace Iberus {

	OpenGLDeferredLightPass::OpenGLDeferredLightPass() {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		auto* shaderObject = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseDeferredLightShader"));
		if (shaderObject) {
			shaderPass = shaderObject;
		}

		GLuint programID{ 0 };
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}

		shaderPass->Bind();
		ShaderBindings::SetUniform<int>(programID, "worldPosOut", 0);
		ShaderBindings::SetUniform<int>(programID, "diffuseOut", 1);
		ShaderBindings::SetUniform<int>(programID, "normalOut", 2);
		ShaderBindings::SetUniform<int>(programID, "uvsOut", 3);

		frameBuffer = dynamic_cast<Framebuffer*>(renderer.GetResource("geometryFBO"));

		quadMesh = dynamic_cast<MeshApi*>(renderer.GetResource("renderQuad"));
	}

	void OpenGLDeferredLightPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) {
		/*glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);*/

		shaderPass->Bind();
		frameBuffer->Bind(FramebufferMode::READING);
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

