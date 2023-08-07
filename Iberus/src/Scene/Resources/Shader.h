#pragma once

#include "Resource.h"

namespace Iberus {

	class Shader : public Resource {
	public:
		Shader(const std::string& inboundID, Buffer vertexBuffer, Buffer fragBuffer);
		~Shader();
	};

}

