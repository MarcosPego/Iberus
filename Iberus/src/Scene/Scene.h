#pragma once

#include "Core.h"
#include "MathUtils.h"
#include "Entity.h"
#include "Camera.h"

using namespace Math;

namespace Iberus {
	class Frame;

	class IBERUS_API Scene {
	public:
		Scene() = default;
		explicit Scene(const std::string& inID);

		virtual ~Scene() = default;

		Scene(const Scene&) = delete;
		Scene& operator= (const Scene&) = delete;

		void Update(double deltaTime);
		void PushDraw(Frame& frame);

		template<typename T = Entity, typename... Args>
		T* CreateEntity(Args&&... args) {
			auto entity = std::unique_ptr<T>(new T(std::forward<Args>(args)...));
			entities.emplace_back(std::move(entity));
			return entities.back().get();
		}

		void AddEntity(const std::string& id, Entity* entity);

	private:
		std::string ID;

		Camera* activeCamera{ nullptr };
		Entity* sceneRoot{ nullptr };

		std::vector<std::unique_ptr<Entity>> entities;
	};

 }
