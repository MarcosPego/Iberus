#include "Enginepch.h"
#include "TransformSystem.h"
#include "Matrix.h"

using namespace Math;

namespace Iberus {

	static EntityId GetParent(World& world, EntityId entity) {
		if (auto* hierarchy = world.GetComponent<HierarchyComponent>(entity)) {
			return hierarchy->ParentId;
		}
		return NullEntity;
	}

	void TransformSystem::Update(World& world) {
		auto* transformStorage = world.GetStorage<TransformComponent>();
		if (!transformStorage) {
			return;
		}

		// Process in parent-before-children order. Multiple passes until all done.
		std::vector<EntityId> remaining;
		for (auto [entityId, _] : *transformStorage) {
			remaining.push_back(entityId);
		}

		int maxPasses = static_cast<int>(remaining.size()) + 1;
		for (int pass = 0; pass < maxPasses && !remaining.empty(); ++pass) {
			std::vector<EntityId> next;
			for (EntityId entityId : remaining) {
				auto* transform = world.GetComponent<TransformComponent>(entityId);
				if (!transform) {
				continue;
			}

				Mat4 localMatrix = MatrixFactory::CreateModelMatrix(
					transform->Position, transform->Rotation, transform->Scale);

				EntityId parentId = GetParent(world, entityId);

				Mat4 worldMatrix;
				if (parentId != NullEntity) {
					auto* parentL2W = world.GetComponent<LocalToWorldComponent>(parentId);
					if (!parentL2W) {
						next.push_back(entityId);
						continue;
					}
					worldMatrix = parentL2W->Matrix * localMatrix;
				} else {
					worldMatrix = localMatrix;
				}

				auto* l2w = world.GetComponent<LocalToWorldComponent>(entityId);
				if (!l2w) {
					world.AddComponent<LocalToWorldComponent>(entityId)->Matrix = worldMatrix;
				} else {
					l2w->Matrix = worldMatrix;
				}
			}
			remaining = std::move(next);
		}
	}

}
