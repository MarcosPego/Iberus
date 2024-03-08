#include "Enginepch.h"
#include "Texture.h"

#include "Engine.h"
#include "RenderCmd.h"
#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Iberus {

	Texture::Texture(const std::string& ID, Buffer inboundBuffer) : Resource(ID) {
		if (Load(std::move(inboundBuffer))) {
			auto& renderer = Engine::Instance()->GetRenderer();
			auto uploadTexture = new UploadTextureRenderCmd(ID, std::move(imageBuffer), width, height, channels);
			renderer.PushRenderCmd(uploadTexture);
		}
	}

	Texture::Texture(const std::string& ID, int inWidth, int inHeight, int inChannels) : Resource(ID) {
		width = inWidth;
		height = inHeight;
		channels = inChannels;

		auto size = ((size_t)width) * ((size_t)height) * ((size_t)channels);

		Buffer bufferData = Buffer(size);
		std::fill_n(bufferData.GetData(), bufferData.GetSize(), 0); // Create black texture

		// TODO allow default color;
		/*const bool alpha = channels == 4;
		for (int i = 0; i < size; i += channels) {	
			bufferData[i] = 255;
			bufferData[i + 1] = 255;
			bufferData[i + 2] = 255;
			if (alpha) {
				bufferData[i + 3] = 255;
			}
		}*/

		auto& renderer = Engine::Instance()->GetRenderer();
		auto uploadTexture = new UploadTextureRenderCmd(ID, std::move(imageBuffer), width, height, channels);
		renderer.PushRenderCmd(uploadTexture);
	}

	Texture::Texture(const std::string& ID, Buffer inboundBuffer, int inWidth, int inHeight, int inChannels) : Resource(ID) {
		width = inWidth;
		height = inHeight;
		channels = inChannels;
		imageBuffer = std::move(inboundBuffer);

		if (imageBuffer.Invalid() || imageBuffer.GetSize() != (width * height * channels)) {
			return;
		}

		auto& renderer = Engine::Instance()->GetRenderer();
		auto uploadTexture = new UploadTextureRenderCmd(ID, std::move(imageBuffer), width, height, channels);
		renderer.PushRenderCmd(uploadTexture);
	}

	Texture::~Texture() {
		auto& renderer = Engine::Instance()->GetRenderer();
		auto deleteTextrure = new DeleteTextureRenderCmd(ID);
		renderer.PushRenderCmd(deleteTextrure);
	}

	bool Texture::Load(Buffer inboundBuffer) {
		std::unique_ptr<uint8_t[]> bufferData;
		bufferData.reset(
			stbi_load_from_memory(static_cast<uint8_t*>(inboundBuffer.GetData()), (int)inboundBuffer.GetSize(), &width, &height, &channels, 0)
		);

		imageBuffer = Buffer(std::move(bufferData), ((size_t)width) * ((size_t)height) * ((size_t)channels));

		if (imageBuffer.Invalid()) {
			return false;
		}

		return true;
	}
}