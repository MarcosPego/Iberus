#include "Enginepch.h"
#include "SceneManager.h"

namespace Iberus {

	Scene* SceneManager::CreateScene(const std::string& sceneID, bool setActive) {
		scenes.emplace(sceneID, new Scene(sceneID));
		auto* scene = scenes[sceneID].get();

		if (setActive) {
			activeScene = scene;
		}

		return scene;
	}

}