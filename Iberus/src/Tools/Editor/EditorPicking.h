#pragma once

#include "EntityId.h"
#include "MathUtils.h"

namespace Iberus {

	class Scene;
	class World;

	/// Pick entity under screen point. Returns NullEntity if none. Uses CPU raycast with AABBs.
	EntityId PickEntityInViewport(Scene& scene, float mouseX, float mouseY,
		float viewportX, float viewportY, float viewportWidth, float viewportHeight,
		const Math::Mat4& viewMatrix, const Math::Mat4& projectionMatrix);

	/// Get world-space AABB for entity. Uses LocalToWorld position with default size.
	void GetEntityAABB(const World& world, EntityId entityId, Math::Vec3& outMin, Math::Vec3& outMax);

}
