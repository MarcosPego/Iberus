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
		

		/// TODO Find and get other scenes;

		//AddScene(const std::string& sceneID, Scene)

	private:

		Scene* activeScene{ nullptr };

		std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
	};

}



