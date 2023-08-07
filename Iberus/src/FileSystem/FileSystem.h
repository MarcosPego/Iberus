#pragma once



namespace Iberus {

	struct Buffer;

	class FileSystem {
		//std::size_t GetRawFileBuffer(const std::string& filename, uint8_t* buffer, std::size_t offset, std::size_t size);

		std::size_t GetRawFileSize(const std::string& filename);

		Buffer GetRawFileBuffer(const std::string& filename);

	};
}


