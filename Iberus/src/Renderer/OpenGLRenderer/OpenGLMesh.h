#pragma once

#include "MeshApi.h"

namespace Iberus {
	class OpenGLMesh final : public MeshApi {
	public:
		OpenGLMesh(const std::string& inboundID, uint32_t inboundHandle,
			const std::vector<Vec3>& vertices, const std::vector<Vec2>& uvs, const std::vector<Vec3>& normals) : MeshApi(inboundID, inboundHandle, vertices, uvs, normals) {
		}

		bool Build(const std::vector<Vec3>& vertices, const std::vector<Vec2>& uvs, const std::vector<Vec3>& normals) final;
		void Destroy() final;

		void Bind() final;
		void Unbind() final;

	private:
		GLuint vaoID{ 0 };
	};
}


