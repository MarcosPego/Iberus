#pragma once

#include "IGUIContext.h"
#include "SceneViewFBO.h"
#include <memory>

namespace Iberus {

	class Editor;

	class SceneViewPanel {
	public:
		explicit SceneViewPanel(Editor& editor);
		void OnDraw(IGUIContext& gui);

	private:
		Editor& editor;
		std::unique_ptr<SceneViewFBO> fbo;
	};

}
