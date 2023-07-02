#pragma once

#include "Entity.h"
#include "MathUtils.h"

#undef near
#undef far

namespace Iberus {

	enum ProjectionType {
		ORTHOGRAFIC,
		PERSPECTIVE
	};


	class Projection {
	public:
		virtual Mat4 GetProjectionMatrix() = 0;
		ProjectionType projectionType;
	};

	class OrthoProjection final : public Projection {
	public:
		float left, right, bottom, top, near, far;
		OrthoProjection(float l, float r, float b, float t, float n, float f);
		Mat4 GetProjectionMatrix() override final;

		ProjectionType projectionType{ ORTHOGRAFIC };
	};

	class PerspectiveProjection final : public Projection {
	public:
		float fovy, aspect, nearZ, farZ;
		PerspectiveProjection(float fovy, float aspect, float nearZ, float farZ);
		Mat4 GetProjectionMatrix() override final;

		ProjectionType projectionType{ PERSPECTIVE };
	};
	
	class IBERUS_API Camera : public Entity {
	};

}



