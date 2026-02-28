#pragma once

#include "Core.h"
#include "Resource.h"

namespace Iberus {

	class IBERUS_API ComputeShader : public Resource {
	public:
		ComputeShader(const std::string& inboundID, Buffer compBuffer);
		~ComputeShader();
	};

}
