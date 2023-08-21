#include "Enginepch.h"
#include "OpenGLRaymarchingPass.h"

#include "Engine.h"
#include "Window.h"
#include "Framebuffer.h"
#include "ShaderBindings.h"
#include "OpenGLShader.h"

namespace Iberus {

	OpenGLRaymarchingPass::OpenGLRaymarchingPass() {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		auto* shaderObject = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseRaymarchingShader"));
		if (shaderObject) {
			shaderPass = shaderObject;
		}

		GLuint programID{ 0 };
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}

		shaderPass->Bind();
		ShaderBindings::SetUniform<int>(programID, "worldPosIn", 4);
		ShaderBindings::SetUniform<int>(programID, "diffuseIn", 5);
		ShaderBindings::SetUniform<int>(programID, "normalIn", 6);
		ShaderBindings::SetUniform<int>(programID, "uvsIn", 7);

		sourceBuffer = dynamic_cast<Framebuffer*>(renderer.GetResource("geometryFBO"));

		textures = { "worldPosOut_2", "diffuseOut_2", "normalOut_2", "uvsOut_2" };
		std::vector<TextureApi*> texturesAPI;
		for (const auto& entry : textures) {
			texturesAPI.push_back(dynamic_cast<TextureApi*>(renderer.GetResource(entry)));
		}
		targetBuffer = renderer.CreateFramebuffer("raymarchFBO", texturesAPI);

		quadMesh = dynamic_cast<MeshApi*>(renderer.GetResource("renderQuad"));
	}

	void OpenGLRaymarchingPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame) {
		/*glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);*/

		shaderPass->Bind();
		sourceBuffer->Bind(FramebufferMode::READING, targetBuffer->GetFBO(), {4, 5, 6, 7});
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
				ShaderBindings::SetUniform<Mat4>(programID, "cameraToWorld", cameraRenderCmd->cameraToWorld);
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