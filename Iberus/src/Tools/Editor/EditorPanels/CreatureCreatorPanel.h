#pragma once

#include "IGUIContext.h"

namespace Iberus {

	class Editor;

	class CreatureCreatorPanel {
	public:
		explicit CreatureCreatorPanel(Editor& editor);
		void OnDraw(IGUIContext& gui);

	private:
		Editor& editor;
	};

}
