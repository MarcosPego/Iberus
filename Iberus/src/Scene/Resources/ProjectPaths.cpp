#include "Enginepch.h"
#include "ProjectPaths.h"
#include "FileSystem.h"

#include <filesystem>

namespace Iberus {

	std::string ProjectPaths::GetAssetsPath() {
		return FileSystem::GetAssetsPath();
	}

	std::string ProjectPaths::GetAssetsRelativePath(const std::string& fullPath) {
		std::string assets = GetAssetsPath();
		std::filesystem::path absPath = std::filesystem::absolute(fullPath);
		std::filesystem::path absAssets = std::filesystem::absolute(assets);
		std::filesystem::path rel;
		try {
			rel = std::filesystem::relative(absPath, absAssets);
			if (rel.empty() || rel.string().find("..") != std::string::npos) {
				return "";
			}
			return rel.generic_string();
		} catch (...) {
			return "";
		}
	}

	std::string ProjectPaths::GetPathRelativeToProject(const std::string& fullPath) {
		std::string projectRoot = FileSystem::GetWorkingDir();
		std::filesystem::path absPath = std::filesystem::absolute(fullPath);
		std::filesystem::path absProject = std::filesystem::absolute(projectRoot);
		try {
			std::filesystem::path rel = std::filesystem::relative(absPath, absProject);
			if (rel.empty() || rel.string().find("..") != std::string::npos) {
				return "";
			}
			return rel.generic_string();
		} catch (...) {
			return "";
		}
	}

}
