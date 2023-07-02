#include "Enginepch.h"
#include "Camera.h"

namespace Iberus {

OrthoProjection::OrthoProjection(float l, float r, float b, float t, float n, float f) {
	left = l;
	right = r;
	bottom = b;
	top = t;
	near = n;
	far = f;
}

Mat4 OrthoProjection::GetProjectionMatrix() {
	return Mat4(
		2 / (right - left), 0, 0, -(right + left) / (right - left),
		0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
		0, 0, 2 / (far - near), -(far + near) / (far - near),
		0, 0, 0, 1
	);
}

PerspectiveProjection::PerspectiveProjection(float fovy, float aspect, float nearZ, float farZ) {
	this->fovy = fovy;
	this->aspect = aspect;
	this->nearZ = nearZ;
	this->farZ = farZ;
}

Mat4 PerspectiveProjection::GetProjectionMatrix() {
	float rad = Deg2Rad(fovy);
	return Mat4(
		1 / (aspect * tan(rad / 2)), 0, 0, 0,
		0, 1 / tan(rad / 2), 0, 0,
		0, 0, -(nearZ + farZ) / (nearZ - farZ), (2 * nearZ * farZ) / (nearZ - farZ),
		0, 0, 1, 0
	);
}

}