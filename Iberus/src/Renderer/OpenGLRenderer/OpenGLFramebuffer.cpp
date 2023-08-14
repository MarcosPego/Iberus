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

	void OpenGLFramebuffer::Bind(FramebufferMode mode) {
		switch (mode) {
		case Iberus::FramebufferMode::WRITING: {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		} break;
		case Iberus::FramebufferMode::READING: {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Zero? Really?

			for (unsigned int i = 0; i < textures.size(); i++) {
				glActiveTexture(GL_TEXTURE0 + i);
				textures[i]->Bind();
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