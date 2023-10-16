#pragma once

#include "Core.h"
#include "MathUtils.h"
#include "Entity.h"
#include "Material.h"
#include "Camera.h"
#include "SDFEntity.h"
#include "Behaviour.h"

using namespace Math;

namespace Iberus {
	struct Frame;

	struct BehaviourPair {
		Entity* entity{ nullptr };
		Behaviour* behaviour{ nullptr };
	};

	class IBERUS_API Scene {
	public:
		Scene() = default;
		explicit Scene(const std::string& inID);

		virtual ~Scene() = default;

		Scene(const Scene&) = delete;
		Scene& operator= (const Scene&) = delete;

		void Update(double deltaTime);
		void PushDraw(Frame& frame);
		void PushDrawSDF(Frame& frame);

		template<typename T = Entity, typename... Args>
		T* CreateEntity(const std::string& ID, Args&&... args) {
			auto entity = std::unique_ptr<T>(new T(ID, this, std::forward<Args>(args)...));
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

		template<typename T = Behaviour>
		bool PushBehaviour(T* behaviour, Entity* entity) {
			if (!behaviour) {
				return false;
			}

			auto behaviourCast = dynamic_cast<Behaviour*>(behaviour);

			if (!behaviourCast) {
				delete behaviour;
				return false;
			}
			
			//const auto index = std::type_index(typeid(T*));

			if (registeredBehaviours.find(behaviourCast->GetType()) == registeredBehaviours.end()) {
				behaviourCast->BindBehaviour(entity, this);
				registeredBehaviours[behaviourCast->GetType()].emplace_back(entity, behaviour);
				return true;
			}

			auto& behaviours = registeredBehaviours.at(behaviourCast->GetType());

			const auto& id = "Behaviour_" + entity->GetID();

			if (std::find_if(behaviours.begin(), behaviours.end(), [id](const BehaviourPair& otherBehaviour) { return id == otherBehaviour.behaviour->GetID(); }) !=
				behaviours.end()) {
				delete behaviour;
				return false;
			}

			behaviourCast->BindBehaviour(entity, this);
			registeredBehaviours[behaviourCast->GetType()].emplace_back(entity, behaviour);
			return true;
		}

		template<typename T = Behaviour>
		void UnbindBehaviour(T* behaviour) {
			auto behaviourCast = dynamic_cast<Behaviour*>(behaviour);

			if (!behaviourCast) {
				return;
			}

			const auto index = behaviourCast->GetType();

			if (registeredBehaviours.find(index) == registeredBehaviours.end()) {
				return;
			}

			auto& behaviours = registeredBehaviours.at(index);
			const auto& id = behaviourCast->GetID();

			behaviours.erase(std::remove_if(behaviours.begin(), behaviours.end(),
				[id](const BehaviourPair& otherBehaviour) { return  id == otherBehaviour.behaviour->GetID(); }), behaviours.end());
		}

		void AddEntity(const std::string& id, Entity* entity);

		Camera* GetActiveCamera() { return activeCamera; }

	private:
		std::string ID;

		Camera* activeCamera{ nullptr };
		Entity* sceneRoot{ nullptr };

		std::unordered_map<std::string, std::unique_ptr<Material>> materials;
		std::vector<std::unique_ptr<Entity>> entities;

		std::map<std::string, std::vector<BehaviourPair>> registeredBehaviours;

		std::vector<SDFEntity*> sdfEntities;
	};

 }
