#pragma once

namespace Iberus {
	template <typename T>
	struct TemplatedBuffer {
		TemplatedBuffer<T>() {
		}

		TemplatedBuffer<T>(std::size_t inboundSize) {
			data = std::make_unique<float[]>(inboundSize);
			size = inboundSize;
		}

		TemplatedBuffer<T>(std::unique_ptr<T[]> inboundData, std::size_t inboundSize) {
			data = std::move(inboundData);
			size = inboundSize;
		}

		std::unique_ptr<T[]> data;
		std::size_t size{0};

		bool Invalid() const {
			return !data || size == 0;
		}
	};

	using Buffer = TemplatedBuffer<uint8_t>;
	using FloatBuffer = TemplatedBuffer<float>;
}

