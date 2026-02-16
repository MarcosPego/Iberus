#pragma once

#include "IGUIContext.h"

namespace Iberus {

	class Editor;

	class AssetInspectorPanel {
	public:
		explicit AssetInspectorPanel(Editor& editor);
		void OnDraw(IGUIContext& gui);

	private:
		void DrawFileInfo(const std::string& path);
		void DrawUnknownAsset(const std::string& path);

		Editor& editor;
	};

}
