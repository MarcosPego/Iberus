#pragma once

#include "RenderObject.h"

namespace Iberus {

	class TextureApi : public RenderObject {
	public:
		TextureApi(const std::string& inboundID, uint32_t inboundHandle, int inWidth, int inHeight, int inChannel);
		virtual ~TextureApi();

	protected:
		virtual void Destroy() {};

		int width{ 0 };
		int height{ 0 };
		int channel{ 0 };
	};

}


