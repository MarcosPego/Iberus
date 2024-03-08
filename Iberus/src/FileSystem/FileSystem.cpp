#include "Enginepch.h"
#include "FileSystem.h"

namespace Iberus {

	std::string FileSystem::GetWorkingDir() {
		const auto path = std::filesystem::current_path();
		const auto u8String = path.u8string();
		const std::string pathString{ u8String.begin(), u8String.end() };
		return pathString;
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