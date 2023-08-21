#include "Enginepch.h"
#include "Camera.h"

#include "RenderBatch.h"
#include "Engine.h"
#include "Window.h"

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
		const float rad = Deg2Rad(fovy * 0.5f);
		const float radTan = (float) tan(rad);
		return Mat4(
			1 / (aspect * radTan), 0, 0, 0,
			0, 1 / radTan, 0, 0,
			0, 0, -(farZ + nearZ) / (farZ - nearZ), -1,
			0, 0, -(2 * farZ * nearZ) / (farZ - nearZ), 0
		);
	}

	Camera::Camera(const std::string& ID) : Entity(ID) {
		
		SetPosition(Vec3(0, 0, 0));
		auto aspectRatio = Iberus::Engine::Instance()->GetCurrentWindow()->GetAspectRatio();

		projection = std::make_unique<PerspectiveProjection>(60, aspectRatio, 0.1f, 8000.0f);
		viewMatrix = MatrixFactory::CreateViewMat4(Vec3(0, 0,-5), GetPosition(), Vec3(0, 1, 0));

		forward = normalize(GetPosition() - Vec3(0, 0, -5));
		up = Vec3(0, 1, 0);

		cameraToWorld = inverse(viewMatrix);
	}


	void Camera::SetPosition(const Vec3& position) {
		Entity::SetPosition(position);
		UpdateViewMatrix();
	}

	void Camera::SetRotation(const Vec3& rotation) {
		Camera::SetRotation(rotation);
		UpdateViewMatrix();
	}

	void Camera::UpdateViewMatrix() {
		// TODO(MPP) look at!
		//forward = Vec3(0, 0, -5);
		//up = Vec3(0, 1, 0);

		//viewMatrix = MatrixFactory::CreateViewMat4(forward, GetPosition(), up);
		//cameraToWorld = inverse(viewMatrix);
	}

	Mat4 Camera::CalculateViewMatrix() {
		UpdateViewMatrix();
		return GetViewMatrix();
	}

}