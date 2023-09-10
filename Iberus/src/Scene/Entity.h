#pragma once

#include "Core.h"
#include "MathUtils.h"

using namespace Math;

namespace Iberus {
	class Mesh;
	class Shader;
	class Material;
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
		explicit Entity(const std::string& ID);

		virtual ~Entity() {};

		//Entity(const Entity&) = delete;
		//Entity& operator= (const Entity&) = delete;

		Entity* GetParent() const { return parent;  }
		void SetParent(Entity* newParent) { parent = newParent; }

		Vec3 GetPosition() { return transform.Position; }
		virtual void SetPosition(const Vec3& position) { transform.Position = position; }

		Vec3 GetRotation() { return transform.Rotation; }
		virtual void SetRotation(const Vec3& rotation) { transform.Rotation = rotation; }

		Vec3 GetScale() { return transform.Scale; }
		virtual void SetScale(const Vec3& scale) { transform.Scale = scale; }

		Transform GetTransform() const { return transform; }

		Mat4 GetModelMatrix() const { 
			if (parent) {
				return MatrixFactory::CreateModelMatrix(transform.Position, transform.Rotation, transform.Scale, parent->GetModelMatrix());
			}
			else {
				return MatrixFactory::CreateModelMatrix(transform.Position, transform.Rotation, transform.Scale);
			}
		}

		void AddEntity(const std::string& id, Entity* entity);

		virtual void PushDraw(RenderBatch& renderBatch);

		void SetMesh(Mesh* inMesh) { mesh = inMesh; }
		Mesh* GetMesh() { return mesh; }

		void SetMaterial(Material* inMaterial) { material = inMaterial; }
		Material* GetMaterial() { return material; }

		const std::unordered_map<std::string, Entity*>& GetChildMap() { return childMap; }

	private:
		std::string ID; // String or unique int ?
		std::string Name;

		Transform transform;

		Entity* parent{ nullptr };
		std::unordered_map<std::string, Entity*> childMap;

		Mesh* mesh{ nullptr };
		Material* material{ nullptr };
	};

}



