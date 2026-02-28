#pragma once

#include "RenderObject.h"

namespace Iberus {

	class ComputeShaderApi : public RenderObject {
	public:
		ComputeShaderApi(const std::string& inboundID, uint32_t inboundHandle) : RenderObject(inboundID, inboundHandle) {
		}

		virtual unsigned int GetProgramID() const = 0;
	};

}
