#include "Enginepch.h"
#include "EditorPicking.h"
#include "Scene.h"
#include "World.h"
#include "Components.h"
#include "Ray.h"
#include "Engine.h"
#include "Mesh.h"

using namespace Math;

namespace Iberus {

	void GetEntityAABB(const World& world, EntityId entityId, Vec3& outMin, Vec3& outMax) {
		auto* l2w = world.GetComponent<LocalToWorldComponent>(entityId);
		if (!l2w) {
			outMin = Vec3(-1, -1, -1);
			outMax = Vec3(1, 1, 1);
			return;
		}
		// Extract position from matrix (column 3 in column-major)
		Vec3 center(l2w->Matrix.data[12], l2w->Matrix.data[13], l2w->Matrix.data[14]);
		// Default size for picking
		float half = 1.0f;
		auto* meshRenderer = world.GetComponent<MeshRendererComponent>(entityId);
		if (meshRenderer && !meshRenderer->MeshId.empty()) {
			auto* mesh = Engine::Instance()->GetResourceManager().GetResource<Mesh>(meshRenderer->MeshId);
			if (mesh) {
				// Mesh has vertices - compute loose AABB in local space then transform
				// For simplicity, use a larger default for mesh entities
				half = 2.0f;
			}
		}
		outMin = center - Vec3(half, half, half);
		outMax = center + Vec3(half, half, half);
	}

	EntityId PickEntityInViewport(Scene& scene, float mouseX, float mouseY,
		float viewportX, float viewportY, float viewportWidth, float viewportHeight,
		const Mat4& viewMatrix, const Mat4& projectionMatrix) {
		Ray ray = RayFromScreen(mouseX, mouseY, viewMatrix, projectionMatrix,
			viewportX, viewportY, viewportWidth, viewportHeight);

		const World& world = scene.GetWorld();
		EntityId sceneRootId = scene.GetSceneRootId();
		if (sceneRootId == NullEntity) {
			return NullEntity;
		}

		EntityId bestEntity = NullEntity;
		float bestT = 1e9f;

		auto pickRecursive = [&](EntityId entityId, auto& self) -> void {
			if (entityId == NullEntity || !world.IsAlive(entityId)) {
				return;
			}
			if (entityId == sceneRootId) {
				// Skip root for selection
			} else {
				Vec3 aabbMin, aabbMax;
				GetEntityAABB(world, entityId, aabbMin, aabbMax);
				float t = RayAABBIntersect(ray, aabbMin, aabbMax);
				if (t >= 0 && t < bestT) {
					bestT = t;
					bestEntity = entityId;
				}
			}
			auto* hierarchy = world.GetComponent<HierarchyComponent>(entityId);
			if (hierarchy) {
				for (EntityId childId : hierarchy->ChildrenIds) {
					self(childId, self);
				}
			}
		};

		pickRecursive(sceneRootId, pickRecursive);
		return bestEntity;
	}

}
