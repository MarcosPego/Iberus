#pragma once

#include "Buffer.h"
#include "MathUtils.h"

#include "RenderObject.h"

using namespace Math;

namespace Iberus {
	class MeshApi : public RenderObject {
	public:
		MeshApi(const std::string& inboundID, uint32_t inboundHandle, 
			const std::vector<Vec3>& vertices, const std::vector<Vec2>& uvs, const std::vector<Vec3>& normals);
		virtual ~MeshApi();

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		size_t VertexSize() const { return vertexSize; }

	protected:
		virtual bool Build(const std::vector<Vec3>& vertices, const std::vector<Vec2>& uvs, const std::vector<Vec3>& normals) { return false; };
		virtual void Destroy() {};

		size_t vertexSize{ 0 };
		/*bool hasUVs;
		bool hasNormals;

		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;*/
	};
}


