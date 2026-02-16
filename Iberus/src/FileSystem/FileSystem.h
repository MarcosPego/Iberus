#pragma once

#include "Buffer.h"
#include <string>
#include <vector>

namespace Iberus {

	struct DirEntry {
		std::string name;
		bool isDirectory;
	};

	class FileSystem {
	public:
		static std::string GetWorkingDir();

		/// Root folder for game assets. Tries "Assets" then "assets" under working dir.
		static std::string GetAssetsPath();

		/// Returns directory entries (subdirs first, then files) for the given path. Empty on error.
		static std::vector<DirEntry> ListDirectory(const std::string& path);

		static std::size_t GetRawFileSize(const std::string& filename);

		static Buffer GetRawFileBuffer(const std::string& filename);
	};
}


