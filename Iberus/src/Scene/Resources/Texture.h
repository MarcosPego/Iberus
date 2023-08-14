#pragma once

#include "Resource.h"

namespace Iberus {

	class IBERUS_API Texture : public Resource {
	public:
		Texture(const std::string& ID, Buffer inboundBuffer);
		Texture(const std::string& ID, int inWidth, int inHeight, int inChannels);
		~Texture();

	private:
		bool Load(Buffer inboundBuffer) override;

		int width;
		int height;
		int channels;
		Buffer imageBuffer;
	};

}



