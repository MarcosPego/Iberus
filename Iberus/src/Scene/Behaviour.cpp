#include "Enginepch.h"
#include "Behaviour.h"

#include "Entity.h"
#include "Scene.h"

namespace Iberus {
	Behaviour::~Behaviour() {
		if (scene) {
			scene->UnbindBehaviour(this);
		}
	}

	void Behaviour::BindBehaviour(Entity* inRoot, Scene* inScene) {
		root = inRoot;
		scene = inScene;

		ID = "Behaviour_" + root->GetID();
	}
}