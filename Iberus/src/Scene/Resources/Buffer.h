#pragma once

namespace Iberus {
	template <typename T>
	struct IBERUS_API TemplatedBuffer {
		TemplatedBuffer<T>() {
		}

		TemplatedBuffer<T>(std::size_t inboundSize) {
			data = std::make_unique<T[]>(inboundSize);
			size = inboundSize;
		}

		TemplatedBuffer<T>(std::unique_ptr<T[]> inboundData, std::size_t inboundSize) {
			data = std::move(inboundData);
			size = inboundSize;
		}

		T* Release() {
			size = 0;
			return data.release();
		}

		void Reset(const TemplatedBuffer<T> inboundBuffer) {
			size = inboundBuffer.GetSize();
			data.reset(inboundBuffer.Release());
		}

		void Reset(T* inboundData = nullptr, std::size_t inboundSize = 0) {
			data.reset(inboundData);
			size = inboundSize;
		}

		void CopyDataFrom(const TemplatedBuffer<T> inboundBuffer) {
			data = std::make_unique<T[]>(inboundBuffer.GetSize());
			size = inboundBuffer.GetSize();
			memcpy(data.get(), inboundBuffer.GetData(), inboundBuffer.GetSize());
		}

		void CopyDataFrom(T* inboundData, std::size_t inboundSize) {
			data = std::make_unique<T[]>(inboundSize);
			size = inboundSize;
			memcpy(data.get(), inboundData, size);
		}

		T* GetData() const {
			return data.get();
		}

		std::size_t GetSize() const {
			return size;
		}

		bool Invalid() const {
			return !data || size == 0;
		}

	private:
		std::unique_ptr<T[]> data;
		std::size_t size{ 0 };
	};

	using Buffer = TemplatedBuffer<uint8_t>;
	using FloatBuffer = TemplatedBuffer<float>;
}

