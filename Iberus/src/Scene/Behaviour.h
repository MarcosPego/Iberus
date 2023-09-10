#pragma once

namespace Iberus {
	class Entity;

	class IBERUS_API Behaviour {
	public:

		virtual void Init() = 0;
		virtual void Update() = 0;

		void BindBehaviour(Entity* inRoot);
		
	protected:
		Entity* root;
		
	};
}

