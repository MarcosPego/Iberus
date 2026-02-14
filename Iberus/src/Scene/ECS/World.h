#pragma once

#include "Core.h"
#include "EntityId.h"
#include "Components.h"
#include "ComponentStorage.h"

#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <memory>

namespace Iberus {

	class IBERUS_API World {
	public:
		World() = default;
		~World() = default;

		World(const World&) = delete;
		World& operator=(const World&) = delete;

		EntityId CreateEntity();
		void DestroyEntity(EntityId entity);

		bool IsAlive(EntityId entity) const;

		template<typename T, typename... Args>
		T* AddComponent(EntityId entity, Args&&... args) {
			auto* storage = GetOrCreateStorage<T>();
			return storage->Emplace(entity, std::forward<Args>(args)...);
		}

		template<typename T>
		T* GetComponent(EntityId entity) {
			auto* storage = GetStorage<T>();
			return storage ? storage->Get(entity) : nullptr;
		}

		template<typename T>
		const T* GetComponent(EntityId entity) const {
			auto* storage = GetStorage<T>();
			return storage ? storage->Get(entity) : nullptr;
		}

		template<typename T>
		bool HasComponent(EntityId entity) const {
			auto* storage = GetStorage<T>();
			return storage && storage->Has(entity);
		}

		template<typename T>
		void RemoveComponent(EntityId entity) {
			auto* storage = GetStorage<T>();
			if (storage) {
				storage->Remove(entity);
			}
		}

		template<typename T>
		ComponentStorage<T>* GetStorage() {
			auto it = storages.find(std::type_index(typeid(T)));
			return it != storages.end() ? static_cast<ComponentStorage<T>*>(it->second.get()) : nullptr;
		}

		template<typename T>
		const ComponentStorage<T>* GetStorage() const {
			auto it = storages.find(std::type_index(typeid(T)));
			return it != storages.end() ? static_cast<const ComponentStorage<T>*>(it->second.get()) : nullptr;
		}

	private:
		template<typename T>
		ComponentStorage<T>* GetOrCreateStorage() {
			auto typeIdx = std::type_index(typeid(T));
			auto it = storages.find(typeIdx);
			if (it == storages.end()) {
				auto storage = std::make_unique<ComponentStorage<T>>();
				auto* ptr = storage.get();
				storages[typeIdx] = std::move(storage);
				return ptr;
			}
			return static_cast<ComponentStorage<T>*>(it->second.get());
		}

		EntityId nextEntityId{ 1 };
		std::unordered_set<EntityId> alive;
		std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> storages;
	};

}
