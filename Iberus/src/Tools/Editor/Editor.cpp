#include "Enginepch.h"
#include "Editor.h"
#include "Engine.h"
#include "KeyCode.h"

#include "imgui.h"

namespace Iberus {

	Editor::Editor()
		: sceneTreePanel(std::make_unique<SceneTreePanel>(*this))
		, sceneViewPanel(std::make_unique<SceneViewPanel>(*this))
		, inspectorPanel(std::make_unique<InspectorPanel>(*this)) {
	}

	Editor::~Editor() = default;

	void Editor::OnUpdate(double deltaTime, IGUIContext* gui) {
		bool f11Pressed = false;
		if (editorMode == EditorMode::Editor) {
			f11Pressed = ImGui::IsKeyPressed(ImGuiKey_F11);
		} else {
			bool f11Down = Engine::Instance()->GetInputManager().IsKeyPressed(KeyCode::F11);
			f11Pressed = f11Down && !wasF11Down;
			wasF11Down = f11Down;
		}
		if (f11Pressed) {
			editorMode = (editorMode == EditorMode::Editor) ? EditorMode::Game : EditorMode::Editor;
			if (editorMode == EditorMode::Game) {
				Engine::Instance()->ClearEditorRenderTarget();
			}
		}

		if (editorMode != EditorMode::Editor || !gui) {
			return;
		}
		gui->BeginDockSpace();
		sceneTreePanel->OnDraw(*gui);
		sceneViewPanel->OnDraw(*gui);
		inspectorPanel->OnDraw(*gui);
		gui->EndDockSpace();
	}

}
