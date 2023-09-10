#include "Enginepch.h"
#include "Behaviour.h"

#include "Entity.h"

namespace Iberus {
	void Behaviour::BindBehaviour(Entity* inRoot) {
		root = inRoot;
	}
}