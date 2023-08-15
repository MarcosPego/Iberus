#include "Enginepch.h"
#include "Material.h"

#include "RenderBatch.h"
#include "RenderCmd.h"
#include "Shader.h"
#include "Renderer.h"
#include "Engine.h"
#include "Texture.h"

namespace Iberus {

	Material::Material(const std::string& inID) {
		ID = inID;
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

	void Material::BindTextures(RenderBatch& renderBatch) {
		for (const auto& entry : texturesBindings) {
			renderBatch.PushRenderCmd(new UniformRenderCmd(entry.first, entry.second, UniformType::INT));
		}
	}

	void Material::PushDraw(RenderBatch& renderBatch) {
		renderBatch.PushRenderCmd(new ShaderRenderCmd(shader->GetID()));
		BindTextures(renderBatch);

		//TODO(MPP) render color!
		renderBatch.PushRenderCmd(new UniformRenderCmd("albedoColor", albedoColor, UniformType::VEC4));

		for (const auto& texture : textures) {
			renderBatch.PushRenderCmd(new TextureRenderCmd(texture.second->GetID(), texture.first, texturesBindings[texture.first]));
		}
	}
}