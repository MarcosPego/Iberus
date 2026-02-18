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
		app->SetGameFullscreen(editor.IsGameFullscreen());
		bool simulating = (editor.GetMode() == EditorMode::Game) && !editor.IsPaused();
		Engine::Instance()->SetSceneSimulationEnabled(simulating);
		if (editor.ConsumeStepRequest()) {
			Engine::Instance()->RequestStepSimulation();
		}
	}

}
