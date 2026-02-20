#include "Enginepch.h"
#include "Ray.h"

using namespace Math;

namespace Math {

	Ray RayFromScreen(float mouseX, float mouseY,
		const Mat4& viewMatrix, const Mat4& projectionMatrix,
		float viewportX, float viewportY, float viewportWidth, float viewportHeight) {
		float ndcX = (2.0f * (mouseX - viewportX) / viewportWidth) - 1.0f;
		float ndcY = 1.0f - (2.0f * (mouseY - viewportY) / viewportHeight);

		Mat4 invViewProj = inverse(projectionMatrix * viewMatrix);
		Vec4 nearPt = invViewProj * Vec4(ndcX, ndcY, -1.0f, 1.0f);
		Vec4 farPt = invViewProj * Vec4(ndcX, ndcY, 1.0f, 1.0f);

		Vec3 origin(nearPt.x / nearPt.w, nearPt.y / nearPt.w, nearPt.z / nearPt.w);
		Vec3 farPoint(farPt.x / farPt.w, farPt.y / farPt.w, farPt.z / farPt.w);
		Vec3 dir = farPoint - origin;
		float len = dir.length();
		if (len > 0.0001f) {
			dir = dir / len;
		} else {
			dir = Vec3(0, 0, -1);
		}
		return Ray{ origin, dir };
	}

	float RayAABBIntersect(const Ray& ray, const Vec3& aabbMin, const Vec3& aabbMax) {
		float invX = ray.Direction.x != 0 ? 1.0f / ray.Direction.x : 1e9f;
		float invY = ray.Direction.y != 0 ? 1.0f / ray.Direction.y : 1e9f;
		float invZ = ray.Direction.z != 0 ? 1.0f / ray.Direction.z : 1e9f;
		Vec3 t0((aabbMin.x - ray.Origin.x) * invX, (aabbMin.y - ray.Origin.y) * invY, (aabbMin.z - ray.Origin.z) * invZ);
		Vec3 t1((aabbMax.x - ray.Origin.x) * invX, (aabbMax.y - ray.Origin.y) * invY, (aabbMax.z - ray.Origin.z) * invZ);
		Vec3 tmin(fminf(t0.x, t1.x), fminf(t0.y, t1.y), fminf(t0.z, t1.z));
		Vec3 tmax(fmaxf(t0.x, t1.x), fmaxf(t0.y, t1.y), fmaxf(t0.z, t1.z));
		float tNear = fmaxf(fmaxf(tmin.x, tmin.y), tmin.z);
		float tFar = fminf(fminf(tmax.x, tmax.y), tmax.z);
		if (tNear <= tFar && tFar >= 0) {
			return tNear >= 0 ? tNear : tFar;
		}
		return -1.0f;
	}

}
