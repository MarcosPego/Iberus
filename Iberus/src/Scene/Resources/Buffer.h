#pragma once

namespace Iberus {
	struct Buffer {
		std::unique_ptr<uint8_t[]> data;
		std::size_t size{0};

		bool Invalid() const {
			return !data || size == 0;
		}
	};
}

