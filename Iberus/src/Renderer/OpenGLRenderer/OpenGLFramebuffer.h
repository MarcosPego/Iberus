#pragma once

#include "Framebuffer.h"

namespace Iberus {
	class TextureApi;

	class OpenGLFramebuffer : public Framebuffer {
	public:
		OpenGLFramebuffer(const std::string& ID, std::vector<TextureApi*> inTextures);
		~OpenGLFramebuffer();

		void Bind() const override;
		void Bind(FramebufferMode mode, int drawTarget, std::vector<int> bindIdxs) override;

		void Unbind() const override;

		GLuint GetFBO() const override { return fbo; }

	private:
		GLuint fbo{ 0 };
		std::vector<TextureApi*> textures;
	};


}

