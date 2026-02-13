#pragma once

#include "Core.h"

namespace Iberus {

	class SceneViewFBO {
	public:
		SceneViewFBO();
		~SceneViewFBO();

		void Resize(int width, int height);
		unsigned int GetFBO() const { return fbo; }
		unsigned int GetTextureID() const { return textureID; }
		int GetWidth() const { return width; }
		int GetHeight() const { return height; }

	private:
		unsigned int fbo{ 0 };
		unsigned int textureID{ 0 };
		int width{ 0 };
		int height{ 0 };
	};

}
