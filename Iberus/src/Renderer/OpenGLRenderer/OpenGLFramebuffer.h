#pragma once

#include "Framebuffer.h"

namespace Iberus {
	class TextureApi;

	class OpenGLFramebuffer : public Framebuffer {
	public:
		OpenGLFramebuffer();
		~OpenGLFramebuffer();

		void Bind(FramebufferMode mode) override;
		void Unbind() override;

	private:
		GLuint fbo{ 0 };
		std::vector<TextureApi*> textures;

	};


}

