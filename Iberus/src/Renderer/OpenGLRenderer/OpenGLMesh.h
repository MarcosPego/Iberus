#pragma once

#include "Mesh.h"

namespace Iberus {
	class OpenGLMesh final : public Mesh {
	public:
		bool Build() final;
		void Destroy() final;

		void Bind() final;
		void Unbind() final;

	private:
		GLuint vaoID{ 0 };
	};
}


