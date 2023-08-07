#pragma once

#include "Buffer.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	class MeshApi {
	public:
		MeshApi(Buffer inboundBuffer);
		virtual ~MeshApi();

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		size_t VertexSize() const { return vertices.size(); }

	protected:
		virtual bool Load(Buffer inboundBuffer);

		virtual bool Build() = 0;
		virtual void Destroy() = 0;

		bool hasUVs;
		bool hasNormals;

		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
	};
}


