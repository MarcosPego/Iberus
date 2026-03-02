#pragma once

#include "Core.h"
#include "Callback.h"
#include "MathUtils.h"
#include "Behaviour.h"
#include "World.h"
#include "EntityId.h"
#include "Components.h"

#include <string>
#include <vector>

using namespace Math;

namespace Iberus {
	struct Frame;

	class IBERUS_API Scene {
	public:
		Scene() = default;
		explicit Scene(const std::string& inID);

		virtual ~Scene() = default;

		Scene(const Scene&) = delete;
		Scene& operator= (const Scene&) = delete;

		void Update(double deltaTime);
		/// Renders scene to the current frame. Camera may be provided by caller; if null, uses scene's active camera.
		void PushDraw(Frame& frame, class CameraRenderCmd* camera = nullptr);
		void PushDrawSDF(Frame& frame);

		// --- ECS API ---
		World& GetWorld() { return world; }
		const World& GetWorld() const { return world; }

		EntityId CreateEntityECS(const std::string& id);
		EntityId GetSceneRootId() const { return sceneRootId; }
		EntityId GetActiveCameraId() const { return activeCameraId; }
		void SetSceneRootId(EntityId id) { sceneRootId = id; }
		void SetActiveCameraId(EntityId id) { activeCameraId = id; }

		template<typename T, typename... Args>
		T* AddComponent(EntityId entity, Args&&... args) {
			return world.AddComponent<T>(entity, std::forward<Args>(args)...);
		}

		template<typename T>
		T* GetComponent(EntityId entity) { return world.GetComponent<T>(entity); }

		template<typename T>
		bool HasComponent(EntityId entity) const { return world.HasComponent<T>(entity); }

		void AddChildECS(EntityId parentId, EntityId childId, const std::string& childTagId);

		/// Remove child from parent's hierarchy (does not destroy the child).
		void RemoveChildFromParent(EntityId parentId, EntityId childId);

		/// Destroy entity and all descendants. Removes from parent's hierarchy.
		void DestroyEntityWithDescendants(EntityId entityId);

		/// Queue entity for destruction at start of next frame (ensures consistent state before render).
		void QueueDestroyEntity(EntityId entityId);
		void FlushPendingDestroys();

		/// Clone entity and all descendants. Returns new root entity. New entity is not parented.
		EntityId CloneEntityWithDescendants(EntityId sourceId);

		/// Generate a unique tag id from base (e.g. "Mesh" -> "Mesh_1" if "Mesh" exists).
		std::string GenerateUniqueTagId(const std::string& base);

		/// Get behaviour type names for an entity (for serialization).
		std::vector<std::string> GetBehavioursForEntity(EntityId entityId) const;

		/// Notify scripts on entity that its structure changed (e.g. creature creator preset).
		void NotifyEntityChanged(EntityId entityId);

		/// Callback invoked when an entity's structure changes (e.g. creature editor). Invoke this or NotifyEntityChanged.
		Callback<EntityId>& OnEntityChanged() { return onEntityChanged; }
		const Callback<EntityId>& OnEntityChanged() const { return onEntityChanged; }

	public:
		template<typename T>
		bool PushBehaviour(EntityId entityId, std::unique_ptr<T> behaviour) {
			if (!behaviour || !world.IsAlive(entityId)) {
				return false;
			}
			auto* b = behaviour.get();
			behaviour->BindBehaviour(entityId, world);
			std::string type = b->GetType();
			std::string tagId;
			if (auto* tag = world.GetComponent<TagComponent>(entityId)) {
				tagId = tag->Id;
			} else {
				tagId = std::to_string(entityId);
			}
			std::string bid = "Behaviour_" + tagId;
			auto& list = registeredBehaviours[type];
			if (std::find_if(list.begin(), list.end(), [&bid](const auto& p) { return p.second->GetID() == bid; }) != list.end()) {
				return false;
			}
			behaviour->Init(entityId, world);
			list.emplace_back(entityId, std::move(behaviour));
			return true;
		}

		template<typename T>
		bool PushBehaviour(EntityId entityId, T* behaviour) {
			return PushBehaviour(entityId, std::unique_ptr<T>(behaviour));
		}

		std::map<std::string, std::vector<std::pair<EntityId, std::unique_ptr<Behaviour>>>>& GetRegisteredBehaviours() { return registeredBehaviours; }
		const std::map<std::string, std::vector<std::pair<EntityId, std::unique_ptr<Behaviour>>>>& GetRegisteredBehaviours() const { return registeredBehaviours; }

		/// Clear all registered behaviours (e.g. before restore from snapshot).
		void ClearRegisteredBehaviours();

	private:
		std::string ID;

		Callback<EntityId> onEntityChanged;
		World world;
		EntityId sceneRootId{ NullEntity };
		EntityId activeCameraId{ NullEntity };

		std::map<std::string, std::vector<std::pair<EntityId, std::unique_ptr<Behaviour>>>> registeredBehaviours;

		std::vector<EntityId> pendingDestroys;
	};

 }
