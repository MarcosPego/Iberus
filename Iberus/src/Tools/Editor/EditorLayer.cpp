#include "Enginepch.h"
#include "EditorLayer.h"
#include "Application.h"

namespace Iberus {

	void EditorLayer::OnUpdate(double deltaTime) {
		Application* app = Application::Get();
		IGUIContext* gui = app->GetGUIContext();
		editor.OnUpdate(deltaTime, gui);
		app->SetEditorMode(editor.GetMode() == EditorMode::Editor);
	}

}
