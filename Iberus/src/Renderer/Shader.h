#pragma once

#include "Buffer.h"

namespace Iberus {
	class Shader {

		virtual void Enable() const = 0;
		virtual void Disable() const = 0;

		virtual bool Load(Buffer vertexBuffer, Buffer fragBuffer) = 0;

	};
}



