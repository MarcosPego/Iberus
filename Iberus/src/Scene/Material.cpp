#include "Enginepch.h"
#include "Material.h"

#include "RenderBatch.h"
#include "RenderCmd.h"
#include "Shader.h"

namespace Iberus {

	Material::Material(const std::string& inID) {
		ID = inID;
	}

	void Material::PushDraw(RenderBatch& renderBatch) {
		renderBatch.PushRenderCmd(new ShaderRenderCmd(shader->GetID()));

		//TODO(MPP) render color!
		renderBatch.PushRenderCmd(new UniformRenderCmd("albedoColor", albedoColor, UniformType::VEC4));
	}
}