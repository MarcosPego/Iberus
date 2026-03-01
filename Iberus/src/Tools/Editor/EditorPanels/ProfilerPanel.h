#pragma once

#include "IGUIContext.h"

namespace Iberus {

	class Editor;

	class ProfilerPanel {
	public:
		explicit ProfilerPanel(Editor& editor);
		void OnDraw(IGUIContext& gui, bool* p_open = nullptr);

	private:
		Editor& editor;
		bool showHeatMap{ false };
	};

}
