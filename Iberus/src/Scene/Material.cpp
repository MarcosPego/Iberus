#include "Enginepch.h"
#include "Material.h"

#include "RenderBatch.h"
#include "RenderCmd.h"
#include "Shader.h"
#include "Renderer.h"
#include "Engine.h"
#include "Texture.h"

namespace Iberus {

	Material::Material(const std::string& inID) : Resource(inID) {
	}

	void Material::SetShader(Shader* inboundShader) {
		shader = inboundShader;

		texturesBindings.clear();
		texturesBindings.emplace(ALBEDOTEXTURE, 5);
		texturesBindings.emplace(NORMALTEXTURE, 6);
		texturesBindings.emplace(METALICTEXTURE, 7);
	}

	void Material::SetTexture(const std::string& ID, Texture* inTexture) {
		textures[ID] = inTexture;
	}

	std::string Material::GetShaderId() const {
		return shader ? shader->GetID() : "";
	}

	std::unordered_map<std::string, std::string> Material::GetTextureSlotPaths() const {
		std::unordered_map<std::string, std::string> out;
		for (const auto& [slot, tex] : textures) {
			if (tex) {
				out[slot] = tex->GetID();
			}
		}
		return out;
	}

	void Material::BindTextures(RenderBatch& renderBatch) {
		for (const auto& entry : texturesBindings) {
			renderBatch.PushRenderCmdToQueue(std::make_unique<UniformRenderCmd<int>>(entry.first, entry.second, UniformType::INT));
		}
	}

	void Material::PushDraw(RenderBatch& renderBatch) {
		renderBatch.PushRenderCmdToQueue(std::make_unique<ShaderRenderCmd>(shader->GetID()));
		BindTextures(renderBatch);

		renderBatch.PushRenderCmdToQueue(std::make_unique<UniformRenderCmd<Vec4>>("albedoColor", albedoColor, UniformType::VEC4));
		bool hasAlbedoTex = (textures.count(ALBEDOTEXTURE) != 0 && textures.at(ALBEDOTEXTURE) != nullptr);
		renderBatch.PushRenderCmdToQueue(std::make_unique<UniformRenderCmd<int>>("hasAlbedoTexture", hasAlbedoTex ? 1 : 0, UniformType::INT));

		for (const auto& texture : textures) {
			renderBatch.PushRenderCmdToQueue(std::make_unique<TextureRenderCmd>(texture.second->GetID(), texture.first, texturesBindings[texture.first]));
		}
	}
}