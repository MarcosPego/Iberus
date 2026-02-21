#include "Enginepch.h"
#include "OpenGLDeferredLightPass.h"

#include "Engine.h"
#include "Window.h"
#include "Framebuffer.h"
#include "Matrix.h"
#include "ShaderBindings.h"
#include "OpenGLShader.h"
#include "RenderCmd.h"

#include "ShaderApi.h"
#include "TextureApi.h"
#include "MeshApi.h"

#include <sstream>

namespace Iberus {

	static void SetLightUniforms(GLuint programID, int index, const LightData& light) {
		std::ostringstream prefix;
		prefix << "lights[" << index << "].";
		std::string p = prefix.str();
		ShaderBindings::SetUniform<int>(programID, (p + "type").c_str(), light.Type);
		ShaderBindings::SetUniform<Vec3>(programID, (p + "color").c_str(), light.Color);
		ShaderBindings::SetUniform<float>(programID, (p + "intensity").c_str(), light.Intensity);
		ShaderBindings::SetUniform<Vec3>(programID, (p + "position").c_str(), light.Position);
		ShaderBindings::SetUniform<float>(programID, (p + "constant").c_str(), light.Constant);
		ShaderBindings::SetUniform<float>(programID, (p + "linear").c_str(), light.Linear);
		ShaderBindings::SetUniform<float>(programID, (p + "quadratic").c_str(), light.Quadratic);
		ShaderBindings::SetUniform<float>(programID, (p + "angel").c_str(), light.Angel);
		ShaderBindings::SetUniform<float>(programID, (p + "cutoffDiameter").c_str(), light.CutoffDiameter);
		ShaderBindings::SetUniform<float>(programID, (p + "range").c_str(), light.Range);
		ShaderBindings::SetUniform<Vec3>(programID, (p + "direction").c_str(), light.Direction);
	}

	OpenGLDeferredLightPass::OpenGLDeferredLightPass() {
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

		quadMesh = dynamic_cast<MeshApi*>(renderer.GetResource("renderQuadNDC"));
	}

	void OpenGLDeferredLightPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame, Framebuffer* source, Framebuffer* target) {
		if (!shaderPass || !source || !target) {
			return;
		}

		shaderPass->Bind();
		source->Bind(FramebufferMode::READING, target->GetFBO(), texturesIdxs);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLuint programID{ 0 };
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}

		if (texturesIdxs.size() == 4) {
			ShaderBindings::SetUniform<int>(programID, "worldPosIn", texturesIdxs.at(0));
			ShaderBindings::SetUniform<int>(programID, "diffuseIn", texturesIdxs.at(1));
			ShaderBindings::SetUniform<int>(programID, "normalIn", texturesIdxs.at(2));
			ShaderBindings::SetUniform<int>(programID, "uvsIn", texturesIdxs.at(3));
		}

		int effW = frame.renderWidth > 0 ? frame.renderWidth : Engine::Instance()->GetEffectiveRenderWidth();
		int effH = frame.renderHeight > 0 ? frame.renderHeight : Engine::Instance()->GetEffectiveRenderHeight();
		if (effW <= 0) { effW = 1; }
		if (effH <= 0) { effH = 1; }
		ShaderBindings::SetUniform<Vec2>(programID, "screenSize", Vec2(static_cast<float>(effW), static_cast<float>(effH)));

		for (const RenderBatch& renderBatch : frame.renderBatches) {
			auto* cameraRenderCmd = renderBatch.GetCameraRenderCmd();
			if (cameraRenderCmd) {
				ShaderBindings::SetUniform<Vec3>(programID, "cameraPos", cameraRenderCmd->cameraPos);
			}

			int lightCount = 0;
			for (const auto& cmd : renderBatch.GetLightRenderCmd()) {
				if (auto* lightsCmd = dynamic_cast<const LightsRenderCmd*>(cmd.get())) {
					lightCount = static_cast<int>(lightsCmd->Lights.size());
					ShaderBindings::SetUniform<int>(programID, "lightCount", lightCount);
					for (int i = 0; i < lightCount; ++i) {
						SetLightUniforms(programID, i, lightsCmd->Lights[i]);
					}
					break;
				}
			}
			if (lightCount == 0) {
				ShaderBindings::SetUniform<int>(programID, "lightCount", 0);
			}
		}

		glDisable(GL_CULL_FACE);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
		if (glGetError() != GL_NO_ERROR) {
			//std::cout << "Error in Mesh" << std::endl;
		}
	}
}

