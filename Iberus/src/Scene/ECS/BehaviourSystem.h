#pragma once

#include "Core.h"
#include "World.h"

namespace Iberus {

	class Scene;

	class IBERUS_API BehaviourSystem {
	public:
		static void Update(World& world, Scene& scene, double deltaTime);

		/// Call when ScriptHost::UnloadAll is invoked to clear cached handles.
		static void ClearScriptHandles();
	};

}
