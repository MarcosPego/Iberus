#pragma once

#include "IGUIContext.h"

namespace Iberus {

	class Editor;

	class InspectorPanel {
	public:
		explicit InspectorPanel(Editor& editor);
		void OnDraw(IGUIContext& gui, bool* p_open = nullptr);

	private:
		Editor& editor;
	};

}
