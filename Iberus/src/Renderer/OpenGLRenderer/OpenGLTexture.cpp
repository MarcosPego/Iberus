#include "Enginepch.h"
#include "OpenGLTexture.h"

namespace Iberus {

	OpenGLTexture::OpenGLTexture(const std::string& inboundID, uint32_t inboundHandle, Buffer inboundBuffer, int inWidth, int inHeight, int inChannel) :
		TextureApi(inboundID, inboundHandle, inWidth, inHeight, inChannel) {

		/// TODO(MPP) keep previous bound texture

		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (inChannel == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, inWidth, inHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, inboundBuffer.GetData());
		}
		else if (inChannel == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, inWidth, inHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, inboundBuffer.GetData());
		}
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::Bind() const {
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	void OpenGLTexture::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	void OpenGLTexture::Destroy() {
		glDeleteTextures(1, &textureID);
	}
}