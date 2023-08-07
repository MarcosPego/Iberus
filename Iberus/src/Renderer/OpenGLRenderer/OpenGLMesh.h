#pragma once

#include "MeshApi.h"

namespace Iberus {
	class OpenGLMesh final : public MeshApi {
	public:
		bool Build() final;
		void Destroy() final;

		void Bind() final;
		void Unbind() final;

	private:
		GLuint vaoID{ 0 };
	};
}


