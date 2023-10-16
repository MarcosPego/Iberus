#pragma once

namespace Iberus {
	class Entity;
	class Scene;

	class IBERUS_API Behaviour {
	public:
		
		virtual ~Behaviour();

		virtual void Init() {};
		virtual void Update(double deltaTime) {};

		void BindBehaviour(Entity* inRoot, Scene* inScene);
		
		const std::string& GetID() const { return ID; };

		const std::string& GetType() const { return type; }
		
	protected:
		std::string type{ "b_behaviour" };
		std::string ID; // String or unique int ?

		Entity* root{ nullptr };
		Scene* scene{nullptr};
	};
}

