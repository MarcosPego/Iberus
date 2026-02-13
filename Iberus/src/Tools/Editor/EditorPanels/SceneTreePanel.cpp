#include "Enginepch.h"
#include "SceneTreePanel.h"
#include "Editor.h"
#include "Engine.h"
#include "Entity.h"
#include "Scene.h"

#include "imgui.h"

namespace Iberus {

	static void DrawEntityTree(Entity* entity, Editor& editor) {
		const char* name = entity->GetID().c_str();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (editor.GetSelectedEntity() == entity) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool opened = ImGui::TreeNodeEx(name, flags);
		if (ImGui::IsItemClicked()) {
			editor.SetSelectedEntity(entity);
		}
		if (opened) {
			for (const auto& [id, child] : entity->GetChildMap()) {
				DrawEntityTree(child, editor);
			}
			ImGui::TreePop();
		}
	}

	SceneTreePanel::SceneTreePanel(Editor& editor) : editor(editor) {
	}

	void SceneTreePanel::OnDraw(IGUIContext& gui) {
		if (!gui.BeginWindow("Scene")) {
			return;
		}
		auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene();
		if (scene && scene->GetSceneRoot()) {
			DrawEntityTree(scene->GetSceneRoot(), editor);
		} else {
			gui.Text("No scene");
		}
		gui.EndWindow();
	}

}
