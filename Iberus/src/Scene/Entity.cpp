#include "Enginepch.h"
#include "Entity.h"

#include "RenderBatch.h"
#include "RenderCmd.h"

#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

#include "Behaviour.h"

#include "Scene.h"

namespace Iberus {
	Entity::Entity(const std::string& inID, Scene* inScene) {
		ID = inID;
		scene = inScene;
	}

	Entity::~Entity() {
		for (auto* behaviour : behaviours) {
			delete behaviour;
		}
	}

	void Entity::AddEntity(const std::string& id, Entity* entity) {
		if (!entity) {
			return;
		}
		// TODO(MPP) Avoid emplancing repeated

		childMap.emplace(id , entity);

	}

	void Entity::PushDraw(RenderBatch& renderBatch) {
		// TODO(MPP) Initial implementation. Might be revised
		if (!active) {
			return;
		}

		if (material) {
			material->PushDraw(renderBatch);

			// Push uniforms here;
			renderBatch.PushRenderCmdToQueue(new UniformRenderCmd("ModelMatrix", GetModelMatrix(), UniformType::MAT4));
		}

		if (mesh) {
			renderBatch.PushRenderCmdToQueue(new MeshRenderCmd(mesh->GetID()));
		}

		for (const auto& child : childMap) {
			child.second->PushDraw(renderBatch);
		}
	}

	void Entity::PushBehaviour(Behaviour* behaviour) {
		if (scene->PushBehaviour(behaviour, this)) {
			behaviour->Init();
			behaviours.push_back(behaviour);
		}
	}
}