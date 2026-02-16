#pragma once

#include "Core.h"

#include <string>

namespace Iberus {

	/// Central location for project/asset paths. Use this for Resources integration.
	class IBERUS_API ProjectPaths {
	public:
		static std::string GetAssetsPath();
		/// Returns path relative to Assets folder, or empty if not under Assets.
		static std::string GetAssetsRelativePath(const std::string& fullPath);
		/// Returns path relative to project root (working dir), for use with FileSystemProvider.
		static std::string GetPathRelativeToProject(const std::string& fullPath);
	};

}
