#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "MathUtils.h"

namespace Math {

	struct Ray {
		Vec3 Origin;
		Vec3 Direction;

		Vec3 At(float t) const { return Origin + Direction * t; }
	};

	/// Unproject screen point to world ray. viewport: (x, y, width, height).
	Ray RayFromScreen(float mouseX, float mouseY,
		const Mat4& viewMatrix, const Mat4& projectionMatrix,
		float viewportX, float viewportY, float viewportWidth, float viewportHeight);

	/// Ray-AABB intersection. Returns t (distance) or -1 if no hit.
	float RayAABBIntersect(const Ray& ray, const Vec3& aabbMin, const Vec3& aabbMax);

}
