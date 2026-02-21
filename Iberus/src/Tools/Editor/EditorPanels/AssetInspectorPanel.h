#pragma once

#include "IGUIContext.h"

#include <memory>
#include <string>

namespace Iberus {

	class Editor;
	class Material;

	class AssetInspectorPanel {
	public:
		explicit AssetInspectorPanel(Editor& editor);
		void OnDraw(IGUIContext& gui);

	private:
		void DrawFileInfo(const std::string& path);
		void DrawMaterialEditor(const std::string& path);
		void DrawUnknownAsset(const std::string& path);

		Editor& editor;
		std::string cachedMaterialPath;
		std::unique_ptr<Material> cachedMaterial;
	};

}
