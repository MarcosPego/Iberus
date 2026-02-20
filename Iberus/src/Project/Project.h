#pragma once

#include "Core.h"

#include <string>
#include <vector>

namespace Iberus {

	class IBERUS_API Project {
	public:
		Project() = default;
		explicit Project(const std::string& name);

		int GetVersion() const { return version; }
		void SetVersion(int v) { version = v; }

		const std::string& GetName() const { return name; }
		void SetName(const std::string& n) { name = n; }

		const std::string& GetRootPath() const { return rootPath; }
		void SetRootPath(const std::string& p) { rootPath = p; }

		const std::string& GetCurrentScenePath() const { return currentScenePath; }
		void SetCurrentScenePath(const std::string& p) { currentScenePath = p; }

		const std::vector<std::string>& GetRecentScenes() const { return recentScenes; }
		void SetRecentScenes(std::vector<std::string> scenes) { recentScenes = std::move(scenes); }
		void AddRecentScene(const std::string& path);

		std::string GetAssetsPath() const;

	private:
		int version{ 1 };
		std::string name;
		std::string rootPath;
		std::string currentScenePath;
		std::vector<std::string> recentScenes;
	};

}
