#include "Enginepch.h"
#include "SceneManager.h"

namespace Iberus {

	Scene* SceneManager::CreateScene(const std::string& sceneID, bool setActive) {
		scenes.emplace(sceneID, std::make_unique<Scene>(sceneID));
		auto* scene = scenes[sceneID].get();

		if (setActive) {
			activeScene = scene;
		}

		return scene;
	}

	Scene* SceneManager::GetScene(const std::string& sceneID) const {
		if (scenes.find(sceneID) != scenes.end()) {
			return scenes.at(sceneID).get();
		}

		IB_CORE_WARN("No scene was found with ID ({})", sceneID);
		return nullptr;
	}

	void SceneManager::Clear() {
		activeScene = nullptr;
		scenes.clear();
	}

}