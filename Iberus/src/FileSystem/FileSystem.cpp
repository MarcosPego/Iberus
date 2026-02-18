#include "Enginepch.h"
#include "FileSystem.h"

#ifdef IB_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Iberus {

	std::string FileSystem::GetExeDirectory() {
#ifdef IB_PLATFORM_WINDOWS
		char path[MAX_PATH];
		if (GetModuleFileNameA(NULL, path, MAX_PATH) > 0) {
			std::filesystem::path p(path);
			auto dir = p.parent_path();
			auto u8 = dir.u8string();
			return std::string{ u8.begin(), u8.end() };
		}
#endif
		return GetWorkingDir();
	}

	std::string FileSystem::GetAppDirectory() {
		return (std::filesystem::path(GetExeDirectory()) / "App").string();
	}

	std::string FileSystem::GetWorkingDir() {
		const auto path = std::filesystem::current_path();
		const auto u8String = path.u8string();
		const std::string pathString{ u8String.begin(), u8String.end() };
		return pathString;
	}

	std::string FileSystem::GetAssetsPath() {
		std::string base = GetWorkingDir();
		std::filesystem::path assetsUpper = std::filesystem::path(base) / "Assets";
		std::filesystem::path assetsLower = std::filesystem::path(base) / "assets";
		if (std::filesystem::exists(assetsUpper) && std::filesystem::is_directory(assetsUpper)) {
			return assetsUpper.string();
		}
		if (std::filesystem::exists(assetsLower) && std::filesystem::is_directory(assetsLower)) {
			return assetsLower.string();
		}
		return assetsUpper.string();
	}

	std::vector<DirEntry> FileSystem::ListDirectory(const std::string& path) {
		std::vector<DirEntry> result;
		try {
			std::filesystem::path p(path);
			if (!std::filesystem::exists(p) || !std::filesystem::is_directory(p)) {
				return result;
			}
			for (const auto& entry : std::filesystem::directory_iterator(p)) {
				DirEntry e;
				auto u8 = entry.path().filename().u8string();
				e.name.assign(u8.begin(), u8.end());
				e.isDirectory = entry.is_directory();
				result.push_back(e);
			}
			std::sort(result.begin(), result.end(), [](const DirEntry& a, const DirEntry& b) {
				if (a.isDirectory != b.isDirectory) {
					return a.isDirectory;
				}
				return a.name < b.name;
			});
		} catch (...) {
		}
		return result;
	}

	std::size_t FileSystem::GetRawFileSize(const std::string& filename) {
		FILE* file;

		if (file = std::fopen(filename.c_str(), "rb")) {
			std::fseek(file, 0, SEEK_END);
			std::size_t size = std::ftell(file);
			std::fseek(file, 0, SEEK_SET);
			std::fclose(file);
			return size;
		}

		return 0;
	}

	Buffer FileSystem::GetRawFileBuffer(const std::string& filename) {
		FILE* file;
		Buffer buffer;

		const auto fullSize = GetRawFileSize(filename);

		std::unique_ptr<uint8_t[]> bufferData = std::make_unique<uint8_t[]>(fullSize);
		if (file = std::fopen(filename.c_str(), "rb")) {
			
			std::fseek(file, 0, SEEK_SET);

			auto size = std::fread(bufferData.get(), sizeof(uint8_t), fullSize, file);
			
			buffer.Reset(bufferData.release(), size);

			std::fclose(file);
		}

		return std::move(buffer);
	}
}