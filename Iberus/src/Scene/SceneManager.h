#pragma once

#include "Scene.h"

namespace Iberus {
	class IBERUS_API SceneManager {
	public:
		SceneManager() = default;
		virtual ~SceneManager() = default;

		SceneManager(const SceneManager&) = delete;
		SceneManager& operator= (const SceneManager&) = delete;

		Scene* CreateScene(const std::string& sceneID, bool setActive = false);

		Scene* GetActiveScene() const { return activeScene; }
		void SetActiveScene(Scene* scene) { activeScene = scene; }

		Scene* GetScene(const std::string& sceneID) const;

	private:

		Scene* activeScene{ nullptr };

		std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
	};

}



