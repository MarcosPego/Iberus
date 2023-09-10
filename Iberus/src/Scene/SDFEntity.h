#pragma once
#include "Entity.h"

namespace Iberus {

	class IBERUS_API SDFEntity : public Entity {
	public:
		explicit SDFEntity(const std::string& ID);
		virtual ~SDFEntity() = default;

		virtual void PushDrawSDF(RenderBatch& renderBatch, int sdfSlot);
		void PushDraw(RenderBatch& renderBatch) override;
			
	private:

		int type{ 1 };
		float radius{ 1.0f };

	};
}



