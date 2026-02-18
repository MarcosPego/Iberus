#pragma once

#include "Buffer.h"
#include <string>
#include <vector>

namespace Iberus {

	struct DirEntry {
		std::string name;
		bool isDirectory;
	};

	class IBERUS_API FileSystem {
	public:
		static std::string GetWorkingDir();

		/// Directory containing the executable.
		static std::string GetExeDirectory();

		/// App runtime directory (exe dir + "/App") where DLLs and configs live.
		static std::string GetAppDirectory();

		/// Root folder for game assets. Tries "Assets" then "assets" under working dir.
		static std::string GetAssetsPath();

		/// Returns directory entries (subdirs first, then files) for the given path. Empty on error.
		static std::vector<DirEntry> ListDirectory(const std::string& path);

		static std::size_t GetRawFileSize(const std::string& filename);

		static Buffer GetRawFileBuffer(const std::string& filename);
	};
}


