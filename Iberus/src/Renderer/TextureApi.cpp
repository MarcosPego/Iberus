#include "Enginepch.h"
#include "TextureApi.h"


namespace Iberus {

	TextureApi::TextureApi(const std::string& inboundID, uint32_t inboundHandle, int inWidth, int inHeight, int inChannel) : RenderObject(inboundID, inboundHandle) {
		width = inWidth;
		height = inHeight;
		channel = inChannel;
	}

	TextureApi::~TextureApi() {
		Destroy();
	}
}