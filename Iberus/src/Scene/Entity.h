#pragma once

#include "Core.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {

	class RenderBatch;

	struct Transform {
		Vec3 Position{ 0,0,0 };
		Vec3 Rotation{ 0,0,0 };
		Vec3 Scale{1,1,1};

		/// TODO(MPP) Pivot
	};

	class IBERUS_API Entity {
	public:
		Entity() = default;
		virtual ~Entity() = default;

		Entity* GetParent() const { return parent;  }
		void SetParent(Entity* newParent) { parent = newParent; }

		Vec3 GetPosition() { return transform.Position; }
		void SetPosition(const Vec3& position) { transform.Position = position; }

		Vec3 GetRotation() { return transform.Rotation; }
		void SetRotation(const Vec3& rotation) { transform.Rotation = rotation; }

		Vec3 GetScale() { return transform.Scale; }
		void SetScale(const Vec3& scale) { transform.Scale = scale; }

		Transform GetTransform() const { return transform; }

		Mat4 GetModelMatrix() const { 
			if (parent) {
				return MatrixFactory::CreateModelMatrix(transform.Position, transform.Rotation, transform.Scale, parent->GetModelMatrix());
			}
			else {
				return MatrixFactory::CreateModelMatrix(transform.Position, transform.Rotation, transform.Scale);
			}
		}

		void PushDraw(RenderBatch& renderBatch);

	private:
		std::string ID; // String or unique int ?
		std::string Name;

		Transform transform;

		Entity* parent{ nullptr };
		std::unordered_map<std::string, Entity*> childMap;
	};

}



