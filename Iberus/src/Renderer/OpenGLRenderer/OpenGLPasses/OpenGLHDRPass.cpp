#include "Enginepch.h"
#include "OpenGLHDRPass.h"

#include "Engine.h"
#include "Framebuffer.h"
#include "RenderSettings.h"
#include "Matrix.h"
#include "ShaderBindings.h"
#include "OpenGLShader.h"
#include "OpenGLFramebuffer.h"

#include "ShaderApi.h"
#include "TextureApi.h"
#include "MeshApi.h"

namespace Iberus {

	OpenGLHDRPass::OpenGLHDRPass() {
		auto& renderer = Iberus::Engine::Instance()->GetRenderer();
		shaderPass = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseHDRShader"));
		brightPassShader = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseBloomBrightShader"));
		blurShader = dynamic_cast<ShaderApi*>(renderer.GetResource("assets/shaders/baseBloomBlurShader"));

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
			ShaderBindings::SetUniform<int>(programID, "bloomIn", bloomTextureUnit);
		}

		ShaderBindings::SetUniform<float>(programID, "exposure", exposure);
		ShaderBindings::SetUniform<float>(programID, "gamma", gamma);
		quadMesh = dynamic_cast<MeshApi*>(renderer.GetResource("renderQuadNDC"));
	}

	OpenGLHDRPass::~OpenGLHDRPass() {
		ReleaseBloomBuffers();
	}

	void OpenGLHDRPass::EnsureBloomBuffers(int width, int height) {
		int quarterW = (width > 0) ? (width / 4) : 256;
		int quarterH = (height > 0) ? (height / 4) : 256;

		if (bloomWidth == quarterW && bloomHeight == quarterH) {
			return;
		}

		ReleaseBloomBuffers();
		bloomWidth = quarterW;
		bloomHeight = quarterH;

		glGenTextures(1, &bloomTex1);
		glBindTexture(GL_TEXTURE_2D, bloomTex1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bloomWidth, bloomHeight, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenTextures(1, &bloomTex2);
		glBindTexture(GL_TEXTURE_2D, bloomTex2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bloomWidth, bloomHeight, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(1, &bloomFBO1);
		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomTex1, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenFramebuffers(1, &bloomFBO2);
		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO2);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomTex2, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLHDRPass::ReleaseBloomBuffers() {
		if (bloomFBO1) {
			glDeleteFramebuffers(1, &bloomFBO1);
			bloomFBO1 = 0;
		}
		if (bloomFBO2) {
			glDeleteFramebuffers(1, &bloomFBO2);
			bloomFBO2 = 0;
		}
		if (bloomTex1) {
			glDeleteTextures(1, &bloomTex1);
			bloomTex1 = 0;
		}
		if (bloomTex2) {
			glDeleteTextures(1, &bloomTex2);
			bloomTex2 = 0;
		}
		bloomWidth = 0;
		bloomHeight = 0;
	}

	void OpenGLHDRPass::ExecutePass(Frame& frame, std::function<void(Frame&, ShaderApi*)> renderFrame, Framebuffer* source, Framebuffer* target) {
		if (!shaderPass || !source || !target) {
			return;
		}

		const PostProcessSettings& settings = Engine::Instance()->GetPostProcessSettings();
		bloomThreshold = settings.bloomThreshold;
		bloomIntensity = settings.enableBloom ? settings.bloomIntensity : 0.0f;
		bloomSpread = settings.bloomSpread;

		int effW = frame.renderWidth > 0 ? frame.renderWidth : Engine::Instance()->GetEffectiveRenderWidth();
		int effH = frame.renderHeight > 0 ? frame.renderHeight : Engine::Instance()->GetEffectiveRenderHeight();

		if (bloomIntensity > 0.0f) {
			EnsureBloomBuffers(effW, effH);

			if (brightPassShader && blurShader && bloomFBO1 && bloomFBO2) {
				source->Bind(FramebufferMode::READING, bloomFBO1, texturesIdxs);

				glViewport(0, 0, bloomWidth, bloomHeight);
				glClearColor(0, 0, 0, 1);
				glClear(GL_COLOR_BUFFER_BIT);

				brightPassShader->Bind();
				GLuint brightProgramID = 0;
				if (auto* openGLBright = dynamic_cast<OpenGLShader*>(brightPassShader); openGLBright) {
					brightProgramID = openGLBright->GetProgramID();
				}
				ShaderBindings::SetUniform<int>(brightProgramID, "colorIn", texturesIdxs.at(1));
				ShaderBindings::SetUniform<float>(brightProgramID, "bloomThreshold", bloomThreshold);

				glDisable(GL_CULL_FACE);
				quadMesh->Bind();
				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());

				const float scaleToBloom = static_cast<float>(bloomWidth) / static_cast<float>(effW);
				float blurRadius = bloomSpread * scaleToBloom;

				blurShader->Bind();
				GLuint blurProgramID = 0;
				if (auto* openGLBlur = dynamic_cast<OpenGLShader*>(blurShader); openGLBlur) {
					blurProgramID = openGLBlur->GetProgramID();
				}
				ShaderBindings::SetUniform<int>(blurProgramID, "bloomIn", 0);
				ShaderBindings::SetUniform<Vec2>(blurProgramID, "screenSize", Vec2(static_cast<float>(bloomWidth), static_cast<float>(bloomHeight)));

				const int blurIterations = 4;
				const float perPassRadius = blurRadius / static_cast<float>(blurIterations);

				for (int iter = 0; iter < blurIterations; iter++) {
					ShaderBindings::SetUniform<float>(blurProgramID, "blurRadius", perPassRadius);

					for (int pass = 0; pass < 2; pass++) {
						GLuint readFBO = (pass == 0) ? bloomFBO1 : bloomFBO2;
						GLuint writeFBO = (pass == 0) ? bloomFBO2 : bloomFBO1;
						GLuint readTex = (pass == 0) ? bloomTex1 : bloomTex2;

						glBindFramebuffer(GL_FRAMEBUFFER, writeFBO);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, readTex);

						bool horizontal = (pass == 0);
						ShaderBindings::SetUniform<Vec2>(blurProgramID, "blurDirection", horizontal ? Vec2(1.0f, 0.0f) : Vec2(0.0f, 1.0f));

						glClear(GL_COLOR_BUFFER_BIT);
						glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());
					}
				}

				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}

		glViewport(0, 0, effW, effH);
		shaderPass->Bind();
		source->Bind(FramebufferMode::READING, target->GetFBO(), texturesIdxs);

		glActiveTexture(GL_TEXTURE0 + bloomTextureUnit);
		if (bloomIntensity > 0.0f && bloomTex1) {
			glBindTexture(GL_TEXTURE_2D, bloomTex1);
		} else {
			static unsigned int blackTex = 0;
			if (blackTex == 0) {
				glGenTextures(1, &blackTex);
				glBindTexture(GL_TEXTURE_2D, blackTex);
				unsigned char black[4] = { 0, 0, 0, 255 };
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, black);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			glBindTexture(GL_TEXTURE_2D, blackTex);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLuint programID{ 0 };
		if (auto* openGLShader = dynamic_cast<OpenGLShader*>(shaderPass); openGLShader) {
			programID = openGLShader->GetProgramID();
		}

		ShaderBindings::SetUniform<Vec2>(programID, "screenSize", Vec2(static_cast<float>(effW), static_cast<float>(effH)));
		ShaderBindings::SetUniform<float>(programID, "exposure", exposure);
		ShaderBindings::SetUniform<float>(programID, "gamma", gamma);
		ShaderBindings::SetUniform<float>(programID, "bloomIntensity", bloomIntensity);

		glDisable(GL_CULL_FACE);
		quadMesh->Bind();
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)quadMesh->VertexSize());

		glActiveTexture(GL_TEXTURE0 + bloomTextureUnit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}
