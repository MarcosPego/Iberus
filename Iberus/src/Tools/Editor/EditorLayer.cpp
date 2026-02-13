#include "Enginepch.h"
#include "EditorLayer.h"
#include "Application.h"

namespace Iberus {

	void EditorLayer::OnUpdate(double deltaTime) {
		IGUIContext* gui = Application::Get()->GetGUIContext();
		if (gui) {
			editor.OnUpdate(deltaTime, gui);
		}
	}

}
