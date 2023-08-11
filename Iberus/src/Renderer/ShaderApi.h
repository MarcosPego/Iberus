#pragma once

#include "Buffer.h"
#include "RenderObject.h"

namespace Iberus {
	class ShaderApi : public RenderObject {
	public:
		ShaderApi() = default;

		ShaderApi(const std::string& inboundID, uint32_t inboundHandle) : RenderObject(inboundID, inboundHandle) {
		}

		virtual bool Load(Buffer vertexBuffer, Buffer fragBuffer) { 
			return false;  
		};
	};
}



