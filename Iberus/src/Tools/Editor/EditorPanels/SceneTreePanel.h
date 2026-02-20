#pragma once

#include "IGUIContext.h"

namespace Iberus {

	class Editor;

	class SceneTreePanel {
	public:
		explicit SceneTreePanel(Editor& editor);
		void OnDraw(IGUIContext& gui);

	private:
		Editor& editor;
	};

}
