#pragma once

#include "IGUIContext.h"

namespace Iberus {

	class Editor;

	class RenderSettingsPanel {
	public:
		explicit RenderSettingsPanel(Editor& editor);
		void OnDraw(IGUIContext& gui, bool* p_open = nullptr);

		void RequestOpen() { requestOpen = true; }

	private:
		void DrawPostProcessSettings();
		void DrawPipelinePasses();

		Editor& editor;
		bool requestOpen{ false };
	};

}
