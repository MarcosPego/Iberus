#include "Enginepch.h"
#include "CameraSystem.h"
#include "Components.h"
#include "RenderBatch.h"
#include "RenderCmd.h"
#include "Matrix.h"
#include "Engine.h"

using namespace Math;

namespace Iberus {

	static Mat4 ComputeViewMatrix(const TransformComponent& transform, Vec3& outForward, Vec3& outUp) {
		const Vec3& pos = transform.Position;
		const Vec3& rot = transform.Rotation;

		float pitchRad = Deg2Rad(rot.x);
		float yawRad = Deg2Rad(rot.y);

		float cosPitch = cosf(pitchRad);
		float sinPitch = sinf(pitchRad);
		float cosYaw = cosf(yawRad);
		float sinYaw = sinf(yawRad);

		outForward = Vec3(sinYaw * cosPitch, -sinPitch, -cosYaw * cosPitch);
		outForward = normalize(outForward);
		outUp = Vec3(0, 1, 0);

		Vec3 center = pos + outForward;
		Mat4 view = MatrixFactory::CreateViewMat4(pos, center, outUp);
		return view;
	}

	static Mat4 GetProjectionMatrix(CameraComponent& cam) {
		if (cam.ProjectionType == CameraProjectionType::Perspective) {
			// Use effective render dimensions (editor viewport when active, else main window)
			cam.PerspectiveParams.Aspect = Engine::Instance()->GetEffectiveRenderAspectRatio();
			const auto& p = cam.PerspectiveParams;
			return MatrixFactory::CreatePerspectiveMat4(p.Fovy, p.Aspect, p.NearZ, p.FarZ);
		}
		const auto& o = cam.OrthoParams;
		return MatrixFactory::CreateOrtoMat4(o.Left, o.Right, o.Bottom, o.Top, o.Near, o.Far);
	}

	EntityId CameraSystem::Execute(World& world, RenderBatch& renderBatch) {
		// Find entity with CameraComponent that is active. Use first camera as active for now.
		auto* cameraStorage = world.GetStorage<CameraComponent>();
		auto* transformStorage = world.GetStorage<TransformComponent>();
		auto* activeStorage = world.GetStorage<ActiveComponent>();
		if (!cameraStorage || !transformStorage) {
			return NullEntity;
		}

		EntityId activeCameraId = NullEntity;
		for (auto [entityId, camera] : *cameraStorage) {
			if (activeStorage) {
				auto* active = activeStorage->Get(entityId);
				if (active && !active->Active) {
				continue;
			}
			}
			activeCameraId = entityId;
			break;
		}
		if (activeCameraId == NullEntity) {
			return NullEntity;
		}

		auto* transform = world.GetComponent<TransformComponent>(activeCameraId);
		auto* camera = world.GetComponent<CameraComponent>(activeCameraId);
		if (!transform || !camera) {
			return NullEntity;
		}

		Vec3 forward, up;
		Mat4 viewMatrix = ComputeViewMatrix(*transform, forward, up);
		camera->ViewMatrix = viewMatrix;
		camera->CameraToWorld = inverse(viewMatrix);

		Mat4 projectionMatrix = GetProjectionMatrix(*camera);

		renderBatch.PushRenderCmdToQueue(
			std::make_unique<CameraRenderCmd>(viewMatrix, projectionMatrix, transform->Position, camera->CameraToWorld),
			CMDQueue::Camera);

		return activeCameraId;
	}

}
