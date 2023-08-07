#pragma once

#include "Buffer.h"
#include "RenderObject.h"

namespace Iberus {
	class ShaderApi : public RenderObject {
	public:
		ShaderApi(const std::string& inboundID, uint32_t inboundHandle, Buffer vertexBuffer, Buffer fragBuffer) : RenderObject(inboundID, inboundHandle) {
			Load(std::move(vertexBuffer), std::move(fragBuffer));
		}

		virtual void Enable() const = 0;
		virtual void Disable() const = 0;

		virtual bool Load(Buffer vertexBuffer, Buffer fragBuffer) { return false;  };
	};
}



