#pragma once

#include "Core.h"
#include "Buffer.h"

namespace Iberus {

	class IBERUS_API PrefabAsset {
	public:
		PrefabAsset() = default;
		explicit PrefabAsset(Buffer buffer);

		const Buffer& GetBuffer() const { return buffer; }
		Buffer& GetBuffer() { return buffer; }
		bool IsValid() const { return !buffer.Invalid(); }

	private:
		Buffer buffer;
	};

}
