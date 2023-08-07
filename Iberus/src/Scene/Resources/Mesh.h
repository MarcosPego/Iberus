#pragma once

#include "Core.h"
#include "Resource.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	struct Buffer;

	class IBERUS_API Mesh : public Resource {
	public:
		Mesh(const std::string& ID, Buffer inboundBuffer);
		~Mesh();
	protected:
		bool Load(Buffer inboundBuffer) override;

		bool hasUVs{ false };
		bool hasNormals{ false };

		std::vector<Vec3> vertices;
		std::vector<Vec2> uvs;
		std::vector<Vec3> normals;
	};
}


