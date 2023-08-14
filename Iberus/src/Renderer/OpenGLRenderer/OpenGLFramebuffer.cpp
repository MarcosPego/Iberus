#include "Enginepch.h"
#include "OpenGLFramebuffer.h"

#include "TextureApi.h"

namespace Iberus {

	OpenGLFramebuffer::OpenGLFramebuffer() {

	}

	OpenGLFramebuffer::~OpenGLFramebuffer() {

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

	void OpenGLFramebuffer::Unbind() {

	}

}