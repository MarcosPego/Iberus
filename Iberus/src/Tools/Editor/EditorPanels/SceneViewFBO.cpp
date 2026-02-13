#include "Enginepch.h"
#include "SceneViewFBO.h"

namespace Iberus {

	SceneViewFBO::SceneViewFBO() {
		glGenFramebuffers(1, &fbo);
		glGenTextures(1, &textureID);
	}

	SceneViewFBO::~SceneViewFBO() {
		if (fbo) {
			glDeleteFramebuffers(1, &fbo);
		}
		if (textureID) {
			glDeleteTextures(1, &textureID);
		}
	}

	void SceneViewFBO::Resize(int w, int h) {
		if (w <= 0 || h <= 0) {
			return;
		}
		if (w == width && h == height) {
			return;
		}
		width = w;
		height = h;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}
