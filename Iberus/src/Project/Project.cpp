#include "Enginepch.h"
#include "Project.h"

#include <filesystem>

namespace Iberus {

	Project::Project(const std::string& projectName) : name(projectName) {
	}

	void Project::AddRecentScene(const std::string& path) {
		if (path.empty()) {
			return;
		}
		recentScenes.erase(
			std::remove(recentScenes.begin(), recentScenes.end(), path),
			recentScenes.end());
		recentScenes.insert(recentScenes.begin(), path);
		constexpr size_t maxRecent = 10;
		if (recentScenes.size() > maxRecent) {
			recentScenes.resize(maxRecent);
		}
	}

	std::string Project::GetAssetsPath() const {
		if (rootPath.empty()) {
			return "";
		}
		std::filesystem::path p(rootPath);
		std::filesystem::path assetsUpper = p / "Assets";
		std::filesystem::path assetsLower = p / "assets";
		if (std::filesystem::exists(assetsUpper) && std::filesystem::is_directory(assetsUpper)) {
			return assetsUpper.string();
		}
		if (std::filesystem::exists(assetsLower) && std::filesystem::is_directory(assetsLower)) {
			return assetsLower.string();
		}
		return assetsUpper.string();
	}

}
