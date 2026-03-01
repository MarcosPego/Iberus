#pragma once

#include "IGUIContext.h"
#include "SceneViewFBO.h"

#include <memory>

namespace Iberus {

	class Editor;

	class SceneViewPanel {
	public:
		explicit SceneViewPanel(Editor& editor);
		void OnDraw(IGUIContext& gui, bool* p_open = nullptr);

	private:
		Editor& editor;
		std::unique_ptr<SceneViewFBO> sceneFbo;
	};

}
