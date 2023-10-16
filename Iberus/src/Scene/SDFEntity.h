#pragma once
#include "Entity.h"

namespace Iberus {

	struct SDFPart {
		Transform transform;
		int type{ 1 };
		float radius{ 1.0f };

		Material* material{ nullptr };

		Vec3 GetPosition() { return transform.Position; }
		virtual void SetPosition(const Vec3& position) { transform.Position = position; }

		Vec3 GetRotation() { return transform.Rotation; }
		virtual void SetRotation(const Vec3& rotation) { transform.Rotation = rotation; }

		Vec3 GetScale() { return transform.Scale; }
		virtual void SetScale(const Vec3& scale) { transform.Scale = scale; }
	};

	class IBERUS_API SDFEntity : public Entity {
	public:
		explicit SDFEntity(const std::string& ID, Scene* inScene);
		virtual ~SDFEntity() = default;

		virtual void PushDrawSDF(RenderBatch& renderBatch, int sdfSlot);
		void PushDraw(RenderBatch& renderBatch) override;
			
		void PushPart(Vec3 transform, int type, float radius, Material* material = nullptr);

	private:
		std::vector<SDFPart> sdfParts;
	};
}



