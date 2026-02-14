#pragma once

#include "Core.h"
#include "MathUtils.h"
#include "Material.h"
#include "Behaviour.h"
#include "World.h"
#include "EntityId.h"
#include "Components.h"

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

		template<typename T = Material, typename... Args>
		T* GetOrCreateMaterial(const std::string& ID, Args&&... args) {
			if (materials.find(ID) == materials.end()) {
				auto material = std::unique_ptr<T>(new T(ID, std::forward<Args>(args)...));
				materials.emplace(ID, std::move(material));
			}		
			return dynamic_cast<T*>(materials.at(ID).get());
		}

		template<typename T>
		bool PushBehaviour(EntityId entityId, std::unique_ptr<T> behaviour) {
			if (!behaviour || !world.IsAlive(entityId)) return false;
			auto* b = behaviour.get();
			behaviour->BindBehaviour(entityId, world);
			std::string type = b->GetType();
			std::string tagId;
			if (auto* tag = world.GetComponent<TagComponent>(entityId)) tagId = tag->Id;
			else tagId = std::to_string(entityId);
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

	private:
		std::string ID;

		World world;
		EntityId sceneRootId{ NullEntity };
		EntityId activeCameraId{ NullEntity };

		std::unordered_map<std::string, std::unique_ptr<Material>> materials;
		std::map<std::string, std::vector<std::pair<EntityId, std::unique_ptr<Behaviour>>>> registeredBehaviours;

		std::vector<EntityId> pendingDestroys;
	};

 }
