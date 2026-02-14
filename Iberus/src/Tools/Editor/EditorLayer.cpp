#include "Enginepch.h"
#include "EditorLayer.h"
#include "Editor.h"
#include "Application.h"
#include "Engine.h"

namespace Iberus {

	void EditorLayer::OnUpdate(double deltaTime) {
		Application* app = Application::Get();
		IGUIContext* gui = app->GetGUIContext();
		editor.OnUpdate(deltaTime, gui);
		app->SetEditorMode(editor.GetMode() == EditorMode::Editor);
		Engine::Instance()->SetCameraOverride(editor.GetEditorCameraOverride());
	}

}
