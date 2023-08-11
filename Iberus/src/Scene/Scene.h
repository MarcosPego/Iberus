#pragma once

#include "Core.h"
#include "MathUtils.h"
#include "Entity.h"
#include "Material.h"
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
			return dynamic_cast<T*>(entities.back().get());
		}

		template<typename T = Material, typename... Args>
		T* GetOrCreateMaterial(const std::string& ID, Args&&... args) {
			if (materials.find(ID) == materials.end()) {
				auto material = std::unique_ptr<T>(new T(ID, std::forward<Args>(args)...));
				materials.emplace(ID, std::move(material));
			}		
			return dynamic_cast<T*>(materials.at(ID).get());
		}

		void AddEntity(const std::string& id, Entity* entity);

		Camera* GetActiveCamera() { return activeCamera; }

	private:
		std::string ID;

		Camera* activeCamera{ nullptr };
		Entity* sceneRoot{ nullptr };

		std::unordered_map<std::string, std::unique_ptr<Material>> materials;
		std::vector<std::unique_ptr<Entity>> entities;
	};

 }
