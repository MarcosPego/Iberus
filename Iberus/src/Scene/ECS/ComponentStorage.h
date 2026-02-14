#pragma once

#include "Core.h"
#include "EntityId.h"

#include <unordered_map>
#include <vector>
#include <typeindex>

namespace Iberus {

	/// Base type-erased component storage.
	class IBERUS_API IComponentStorage {
	public:
		virtual ~IComponentStorage() = default;
		virtual bool Has(EntityId entity) const = 0;
		virtual void Remove(EntityId entity) = 0;
		virtual void Clear() = 0;
	};


	/// Dense vector storage with sparse index for O(1) lookup.
	/// Components stored contiguously for cache-friendly iteration.
	/// Remove uses swap-with-last for O(1) deletion.
	template<typename T>
	class ComponentStorage : public IComponentStorage {
	public:
		T* Add(EntityId entity, T component) {
			return Emplace(entity, std::move(component));
		}

		template<typename... Args>
		T* Emplace(EntityId entity, Args&&... args) {
			auto it = entityToIndex.find(entity);
			if (it != entityToIndex.end()) {
				components[it->second] = T(std::forward<Args>(args)...);
				return &components[it->second];
			}
			size_t index = components.size();
			entities.push_back(entity);
			components.emplace_back(std::forward<Args>(args)...);
			entityToIndex[entity] = index;
			return &components[index];
		}

		T* Get(EntityId entity) {
			auto it = entityToIndex.find(entity);
			return it != entityToIndex.end() ? &components[it->second] : nullptr;
		}

		const T* Get(EntityId entity) const {
			auto it = entityToIndex.find(entity);
			return it != entityToIndex.end() ? &components[it->second] : nullptr;
		}

		bool Has(EntityId entity) const override {
			return entityToIndex.find(entity) != entityToIndex.end();
		}

		void Remove(EntityId entity) override {
			auto it = entityToIndex.find(entity);
			if (it == entityToIndex.end()) {
				return;
			}
			size_t index = it->second;
			entityToIndex.erase(it);

			size_t last = components.size() - 1;
			if (index != last) {
				EntityId movedEntity = entities[last];
				entities[index] = movedEntity;
				components[index] = std::move(components[last]);
				entityToIndex[movedEntity] = index;
			}
			entities.pop_back();
			components.pop_back();
		}

		void Clear() override {
			entities.clear();
			components.clear();
			entityToIndex.clear();
		}

		size_t Size() const {
			return components.size();
		}

		class Iterator {
		public:
			using value_type = std::pair<EntityId, T&>;
			using reference = value_type;

			Iterator(std::vector<EntityId>* ents, std::vector<T>* comps, size_t i)
				: entities(ents), components(comps), index(i) {}

			reference operator*() {
				return { (*entities)[index], (*components)[index] };
			}

			Iterator& operator++() {
				++index;
				return *this;
			}

			bool operator!=(const Iterator& other) const {
				return index != other.index;
			}

		private:
			std::vector<EntityId>* entities;
			std::vector<T>* components;
			size_t index;
		};

		class ConstIterator {
		public:
			using value_type = std::pair<EntityId, const T&>;
			using reference = value_type;

			ConstIterator(const std::vector<EntityId>* ents, const std::vector<T>* comps, size_t i)
				: entities(ents), components(comps), index(i) {}

			reference operator*() {
				return { (*entities)[index], (*components)[index] };
			}

			ConstIterator& operator++() {
				++index;
				return *this;
			}

			bool operator!=(const ConstIterator& other) const {
				return index != other.index;
			}

		private:
			const std::vector<EntityId>* entities;
			const std::vector<T>* components;
			size_t index;
		};

		Iterator begin() {
			return Iterator(&entities, &components, 0);
		}

		Iterator end() {
			return Iterator(&entities, &components, components.size());
		}

		ConstIterator begin() const {
			return ConstIterator(&entities, &components, 0);
		}

		ConstIterator end() const {
			return ConstIterator(&entities, &components, components.size());
		}

	private:
		std::vector<EntityId> entities;
		std::vector<T> components;
		std::unordered_map<EntityId, size_t> entityToIndex;
	};

}
