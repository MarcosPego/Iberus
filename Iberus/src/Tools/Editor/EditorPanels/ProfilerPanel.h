#pragma once

#include "IGUIContext.h"

namespace Iberus {

	class Editor;

	class ProfilerPanel {
	public:
		explicit ProfilerPanel(Editor& editor);
		void OnDraw(IGUIContext& gui);

	private:
		Editor& editor;
		bool showHeatMap{ false };
	};

}
