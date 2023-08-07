#pragma once

namespace Iberus {
	class FileSystem {
	public:
		static std::size_t GetRawFileSize(const std::string& filename);

		static Buffer GetRawFileBuffer(const std::string& filename);

	};
}


