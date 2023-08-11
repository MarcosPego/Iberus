#pragma once

#include "Resource.h"

namespace Iberus {

	class IBERUS_API Texture : public Resource {
	public:
		Texture(const std::string& ID, Buffer inboundBuffer);
		~Texture();

	private:
		bool Load(Buffer inboundBuffer) override;

		int width;
		int height;
		int channels;
		Buffer imageBuffer;
	};

}



