#pragma once

#include "Buffer.h"
#include "RenderObject.h"

namespace Iberus {
	class ShaderApi : public RenderObject {
	public:
		virtual void Enable() const = 0;
		virtual void Disable() const = 0;

		virtual bool Load(Buffer vertexBuffer, Buffer fragBuffer) = 0;
	};
}



