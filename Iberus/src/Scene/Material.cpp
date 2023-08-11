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

		texturesBindings.emplace(ALBEDOTEXTURE, 0);
		texturesBindings.emplace(NORMALTEXTURE, 1);
		texturesBindings.emplace(METALICTEXTURE, 2);

		/// Note(MPP) The convention is that every base shader for a material should have albedo, normal and metalic texture
		BindTextures();
	}

	void Material::SetTexture(const std::string& ID, Texture* inTexture) {
		textures[ID] = inTexture;
	}

	void Material::BindTextures() {
		auto& renderer = Engine::Instance()->GetRenderer();
		renderer.PushRenderCmd(new ShaderRenderCmd(shader->GetID()));
		for (const auto& entry : texturesBindings) {
			renderer.PushRenderCmd(new UniformRenderCmd(entry.first, entry.second, UniformType::INT));
		}
	}

	void Material::PushDraw(RenderBatch& renderBatch) {
		renderBatch.PushRenderCmd(new ShaderRenderCmd(shader->GetID()));

		//TODO(MPP) render color!
		renderBatch.PushRenderCmd(new UniformRenderCmd("albedoColor", albedoColor, UniformType::VEC4));

		for (const auto& texture : textures) {
			renderBatch.PushRenderCmd(new TextureRenderCmd(texture.second->GetID(), texture.first, texturesBindings[texture.first]));
		}
	}
}