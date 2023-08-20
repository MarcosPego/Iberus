#include "Enginepch.h"
#include "OpenGLFramebuffer.h"

#include "OpenGLTexture.h"
#include "Engine.h"
#include "Window.h"

namespace Iberus {

	OpenGLFramebuffer::OpenGLFramebuffer(const std::string& ID, std::vector<TextureApi*> inTextures) : Framebuffer(ID) {
		textures = inTextures;

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

		for (unsigned int i = 0; i < textures.size(); i++) {
			textures[i]->Bind();

			auto* currentWindow = Engine::Instance()->GetCurrentWindow();

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, currentWindow->GetWidth(), currentWindow->GetHeight(), 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			auto* openGLTexture = dynamic_cast<OpenGLTexture*>(textures[i]);
			if (openGLTexture) {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, openGLTexture->GetOpenGLID(), 0);
			} // TODO(MPP) Log that it didnt cast well
		}
		std::vector<unsigned int> attachments;

		/// Temp
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
							GL_COLOR_ATTACHMENT1,
							GL_COLOR_ATTACHMENT2,
							GL_COLOR_ATTACHMENT3 };

		glDrawBuffers(4, DrawBuffers);

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status: 0x%x\n", Status);
			//return false;
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // TODO(MPP) Restore previews fbo instead of default
	}

	OpenGLFramebuffer::~OpenGLFramebuffer() {
		if (fbo != 0) {
			glDeleteFramebuffers(1, &fbo);
		}
	}

	void OpenGLFramebuffer::Bind() const {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	}

	void OpenGLFramebuffer::Bind(FramebufferMode mode, int drawTarget, std::vector<int> bindIdxs) {
		switch (mode) {
		case Iberus::FramebufferMode::WRITING: {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		} break;
		case Iberus::FramebufferMode::READING: {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawTarget);

			if (!bindIdxs.empty() && bindIdxs.size() == textures.size()) { // TODO(MPP) we might not need it to be same size
				for (unsigned int i = 0; i < textures.size(); i++) {
					glActiveTexture(GL_TEXTURE0 + bindIdxs[i]);
					textures[i]->Bind();
				}
			}
			else {
				for (unsigned int i = 0; i < textures.size(); i++) {
					glActiveTexture(GL_TEXTURE0 + i);
					textures[i]->Bind();
				}
			}
		} break;
		default:
			break;
		}

	}

	void OpenGLFramebuffer::Unbind() const {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

}