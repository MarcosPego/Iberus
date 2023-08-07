#pragma once

#include "Buffer.h"

namespace Iberus {

	class IProvider {
	public:
		virtual Buffer GetRawFileBuffer(const std::string& filename) = 0;

	};

}


