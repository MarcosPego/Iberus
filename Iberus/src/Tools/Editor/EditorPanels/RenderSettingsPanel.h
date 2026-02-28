#pragma once

#include "IGUIContext.h"

namespace Iberus {

	class Editor;

	class RenderSettingsPanel {
	public:
		explicit RenderSettingsPanel(Editor& editor);
		void OnDraw(IGUIContext& gui);

		void RequestOpen() { requestOpen = true; }

	private:
		void DrawPostProcessSettings();
		void DrawSDFSettings();
		void DrawPipelinePasses();

		Editor& editor;
		bool requestOpen{ false };
	};

}
