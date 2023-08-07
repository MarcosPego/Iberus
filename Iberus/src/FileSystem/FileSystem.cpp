#include "Enginepch.h"
#include "FileSystem.h"

#include "Buffer.h"

namespace Iberus {

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

		auto fullSize = GetRawFileSize(filename);

		std::unique_ptr<uint8_t[]> bufferData = std::make_unique<uint8_t[]>(fullSize);
		if (file = std::fopen(filename.c_str(), "rb")) {
			
			std::fseek(file, 0, SEEK_SET);

			auto size = std::fread(bufferData.get(), sizeof(uint8_t), fullSize, file);
			
			buffer.data = std::move(bufferData);
			buffer.size = size;

			std::fclose(file);
		}

		return std::move(buffer);
	}
}