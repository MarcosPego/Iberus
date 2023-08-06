#include "Enginepch.h"
#include "Entity.h"

#include "RenderBatch.h"

namespace Iberus {
	void Entity::PushDraw(RenderBatch& renderBatch) {

		for (const auto& child : childMap) {
			child.second->PushDraw(renderBatch);
		}
	}
}