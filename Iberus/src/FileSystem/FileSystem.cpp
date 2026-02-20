#include "Enginepch.h"
#include "FileSystem.h"

#ifdef IB_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Iberus {

	static std::string PathToUtf8(const std::filesystem::path& p) {
		auto u8 = p.u8string();
		return std::string(reinterpret_cast<const char*>(u8.data()), u8.size());
	}

	std::string FileSystem::GetExeDirectory() {
#ifdef IB_PLATFORM_WINDOWS
		char path[MAX_PATH];
		if (GetModuleFileNameA(NULL, path, MAX_PATH) > 0) {
			std::filesystem::path p(path);
			return PathToUtf8(p.parent_path());
		}
#endif
		return GetWorkingDir();
	}

	std::string FileSystem::GetAppDirectory() {
		return PathToUtf8(std::filesystem::path(GetExeDirectory()) / "App");
	}

	std::string FileSystem::GetWorkingDir() {
		return PathToUtf8(std::filesystem::current_path());
	}

	std::string FileSystem::GetAssetsPath() {
		std::string base = GetWorkingDir();
		std::filesystem::path assetsUpper = std::filesystem::path(base) / "Assets";
		std::filesystem::path assetsLower = std::filesystem::path(base) / "assets";
		if (std::filesystem::exists(assetsUpper) && std::filesystem::is_directory(assetsUpper)) {
			return PathToUtf8(assetsUpper);
		}
		if (std::filesystem::exists(assetsLower) && std::filesystem::is_directory(assetsLower)) {
			return PathToUtf8(assetsLower);
		}
		return PathToUtf8(assetsUpper);
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
				e.name.assign(reinterpret_cast<const char*>(u8.data()), u8.size());
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
#ifdef IB_PLATFORM_WINDOWS
		FILE* file = nullptr;
		if (fopen_s(&file, filename.c_str(), "rb") == 0 && file) {
#else
		if (FILE* file = std::fopen(filename.c_str(), "rb")) {
#endif
			std::fseek(file, 0, SEEK_END);
			std::size_t size = static_cast<std::size_t>(std::ftell(file));
			std::fseek(file, 0, SEEK_SET);
			std::fclose(file);
			return size;
		}
		return 0;
	}

	Buffer FileSystem::GetRawFileBuffer(const std::string& filename) {
		Buffer buffer;
		const auto fullSize = GetRawFileSize(filename);

		std::unique_ptr<uint8_t[]> bufferData = std::make_unique<uint8_t[]>(fullSize);
#ifdef IB_PLATFORM_WINDOWS
		FILE* file = nullptr;
		if (fopen_s(&file, filename.c_str(), "rb") == 0 && file) {
#else
		if (FILE* file = std::fopen(filename.c_str(), "rb")) {
#endif
			std::fseek(file, 0, SEEK_SET);
			auto size = std::fread(bufferData.get(), sizeof(uint8_t), fullSize, file);
			buffer.Reset(bufferData.release(), static_cast<std::size_t>(size));
			std::fclose(file);
		}
		return buffer;
	}
}