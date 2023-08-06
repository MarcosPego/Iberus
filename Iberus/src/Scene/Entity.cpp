#include "Enginepch.h"
#include "Entity.h"

#include "RenderBatch.h"

#include "Material.h"
#include "Mesh.h"

namespace Iberus {
	void Entity::PushDraw(RenderBatch& renderBatch) {
		// TODO(MPP) Initial implementation. Might be revised

		if (material) {
			renderBatch.PushRenderCmd(new ShaderRenderCmd(material->GetShader()));

			// Push uniforms here;
			renderBatch.PushRenderCmd(new UniformRenderCmd("ModelMatrix", GetModelMatrix(), UniformType::MAT4));
			//renderBatch.PushRenderCmd(new UniformRenderCmd("color", GetModelMatrix, UniformType::COLOR));
		}

		if (mesh) {
			renderBatch.PushRenderCmd(new MeshRenderCmd(mesh));
		}

		for (const auto& child : childMap) {
			child.second->PushDraw(renderBatch);
		}
	}
}