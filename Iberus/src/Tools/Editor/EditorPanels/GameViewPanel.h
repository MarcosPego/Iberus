#pragma once

#include "IGUIContext.h"
#include "SceneViewFBO.h"

#include <memory>

namespace Iberus {

	class Editor;

	class GameViewPanel {
	public:
		explicit GameViewPanel(Editor& editor);
		void OnDraw(IGUIContext& gui, bool* p_open = nullptr);

	private:
		Editor& editor;
		std::unique_ptr<SceneViewFBO> gameFbo;
	};

}
