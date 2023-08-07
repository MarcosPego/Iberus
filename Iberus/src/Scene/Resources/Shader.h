#pragma once

#include "Core.h"
#include "Resource.h"

namespace Iberus {

	class IBERUS_API Shader : public Resource {
	public:
		Shader(const std::string& inboundID, Buffer vertexBuffer, Buffer fragBuffer);
		~Shader();
	};

}

