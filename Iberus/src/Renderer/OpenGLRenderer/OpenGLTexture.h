#pragma once

#include "TextureApi.h"

namespace Iberus {

	class OpenGLTexture : public TextureApi {
	public:
		OpenGLTexture(const std::string& inboundID, uint32_t inboundHandle, Buffer inboundBuffer, int inWidth, int inHeight, int inChannel);

		void Bind() const final;
		void Unbind() const final;

		GLuint GetOpenGLID() const { return textureID; }

	private:
		void Destroy() final;

		GLuint textureID{ 0 };
	};

}



