#pragma once

#include "Entity.h"
#include "MathUtils.h"

#undef near
#undef far

namespace Iberus {
	
	enum class ProjectionType {
		UNKNOWN,
		ORTHOGRAFIC,
		PERSPECTIVE
	};


	class Projection {
	public:
		virtual Mat4 GetProjectionMatrix() = 0;
		ProjectionType projectionType{ ProjectionType::UNKNOWN };
	};

	class OrthoProjection final : public Projection {
	public:
		float left, right, bottom, top, near, far;
		OrthoProjection(float l, float r, float b, float t, float n, float f);
		Mat4 GetProjectionMatrix() override final;

		ProjectionType projectionType{ ProjectionType::ORTHOGRAFIC };
	};

	class PerspectiveProjection final : public Projection {
	public:
		float fovy, aspect, nearZ, farZ;
		PerspectiveProjection(float fovy, float aspect, float nearZ, float farZ);
		Mat4 GetProjectionMatrix() override final;

		ProjectionType projectionType{ ProjectionType::PERSPECTIVE };
	};
	
	class IBERUS_API Camera : public Entity {
	public:
		explicit Camera(const std::string& ID, Scene* inScene);
		virtual ~Camera() = default;

		Mat4 GetViewMatrix() const { return viewMatrix; }
		Mat4 GetProjectionMatrix() const { return projection->GetProjectionMatrix(); }

		Mat4 GetCameraToWorld() const { return cameraToWorld; }

		void SetPosition(const Vec3& position) override;
		void SetRotation(const Vec3& rotation) override;
		void SetScale(const Vec3& scale) override { }

		Mat4 CalculateViewMatrix();

	private:
		void UpdateViewMatrix();

		Vec3 forward;
		Vec3 up;

		Mat4 viewMatrix;
		Mat4 cameraToWorld;

		std::unique_ptr<Projection> projection;
	};

}



