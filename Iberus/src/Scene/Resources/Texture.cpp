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

	Texture::~Texture() {
		auto& renderer = Engine::Instance()->GetRenderer();
		auto deleteTextrure = new DeleteTextureRenderCmd(ID);
		renderer.PushRenderCmd(deleteTextrure);
	}

	bool Texture::Load(Buffer inboundBuffer) {
		std::unique_ptr<uint8_t[]> bufferData;
		bufferData.reset(
			stbi_load_from_memory(static_cast<uint8_t*>(inboundBuffer.data.get()), (int)inboundBuffer.size, &width, &height, &channels, 0)
		);

		imageBuffer.data = std::move(bufferData);
		imageBuffer.size = ((size_t) width) * ((size_t) height) * ((size_t) channels);

		if (imageBuffer.Invalid()) {
			return false;
		}

		return true;
	}
}