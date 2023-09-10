#include "Enginepch.h"
#include "Entity.h"

#include "RenderBatch.h"
#include "RenderCmd.h"

#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

#include "Behaviour.h"

namespace Iberus {
	Entity::Entity(const std::string& inID) {
		ID = inID;
	}

	Entity::~Entity() {
		for (auto* behaviour : behaviours) {
			delete behaviour;
		}
	}

	void Entity::Upadate() {
		for (const auto& behaviour : behaviours) {
			behaviour->Update();
		}

		for (const auto& child : childMap) {
			child.second->Upadate();
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
		behaviour->BindBehaviour(this);
		behaviours.push_back(behaviour);
	}
}