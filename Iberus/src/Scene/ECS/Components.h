#pragma once

#include "Core.h"
#include "MathUtils.h"
#include "EntityId.h"

using namespace Math;

namespace Iberus {

	struct IBERUS_API TransformComponent {
		Vec3 Position{ 0, 0, 0 };
		Vec3 Rotation{ 0, 0, 0 };
		Vec3 Scale{ 1, 1, 1 };
	};

	/// Computed by TransformSystem from hierarchy.
	struct IBERUS_API LocalToWorldComponent {
		Mat4 Matrix;
	};

	struct IBERUS_API HierarchyComponent {
		EntityId ParentId{ NullEntity };
		std::vector<EntityId> ChildrenIds;
	};

	struct IBERUS_API TagComponent {
		std::string Id;
		std::string Name;
		TagComponent() = default;
		TagComponent(std::string id, std::string name) : Id(std::move(id)), Name(std::move(name)) {}
	};

	struct IBERUS_API ActiveComponent {
		bool Active{ true };
		ActiveComponent() = default;
		explicit ActiveComponent(bool active) : Active(active) {}
	};

	/// Reference resources by ID (path), not pointer.
	struct IBERUS_API MeshRendererComponent {
		std::string MeshId;
		std::string MaterialId;
	};

	enum class CameraProjectionType {
		Unknown,
		Orthographic,
		Perspective
	};

	/// Camera projection parameters (perspective).
	struct IBERUS_API PerspectiveProjectionParams {
		float Fovy{ 60.0f };
		float Aspect{ 16.0f / 9.0f };
		float NearZ{ 0.1f };
		float FarZ{ 8000.0f };
	};

	/// Camera projection parameters (ortho).
	struct IBERUS_API OrthoProjectionParams {
		float Left{ -1 };
		float Right{ 1 };
		float Bottom{ -1 };
		float Top{ 1 };
		float Near{ -1 };
		float Far{ 1 };
	};

	struct IBERUS_API CameraComponent {
		CameraProjectionType ProjectionType{ CameraProjectionType::Perspective };
		PerspectiveProjectionParams PerspectiveParams;
		OrthoProjectionParams OrthoParams;
		Mat4 ViewMatrix;
		Mat4 CameraToWorld;
	};

	struct IBERUS_API SDFPartData {
		TransformComponent Transform;
		int Type{ 1 };
		float Radius{ 1.0f };
		std::string MaterialId; // Override material per part; empty = use entity material
	};

	struct IBERUS_API SDFComponent {
		std::vector<SDFPartData> Parts;
	};

	/// Light type matches shader: 1=Point, 2=Spot, 3=Dir, 4=Area
	enum class LightType : int {
		Point = 1,
		Spot = 2,
		Directional = 3,
		Area = 4
	};

	struct IBERUS_API LightComponent {
		LightType Type{ LightType::Point };
		Vec3 Color{ 1, 1, 1 };
		float Intensity{ 1.0f };

		float Constant{ 1.0f };
		float Linear{ 0.09f };
		float Quadratic{ 0.032f };

		float Range{ 50.0f };

		float SpotAngle{ 45.0f };
		float SpotCutoffDiameter{ 0.0f };

		Vec3 Direction{ 0, -1, 0 };
	};

}
