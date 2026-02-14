#include "Enginepch.h"
#include "SceneTreePanel.h"
#include "Editor.h"
#include "Engine.h"
#include "Scene.h"
#include "World.h"
#include "Components.h"

#include "imgui.h"

namespace Iberus {

	static void DrawEntityTree(World& world, EntityId entityId, Editor& editor) {
		if (entityId == NullEntity || !world.IsAlive(entityId)) return;
		auto* tag = world.GetComponent<TagComponent>(entityId);
		const char* name = tag ? tag->Id.c_str() : "?";
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (editor.GetSelectedEntityId() == entityId) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool opened = ImGui::TreeNodeEx(name, flags);
		if (ImGui::IsItemClicked()) {
			editor.SetSelectedEntity(entityId);
		}
		if (opened) {
			auto* hierarchy = world.GetComponent<HierarchyComponent>(entityId);
			if (hierarchy) {
				for (EntityId childId : hierarchy->ChildrenIds) {
					DrawEntityTree(world, childId, editor);
				}
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
		if (scene) {
			EntityId rootId = scene->GetSceneRootId();
			if (rootId != NullEntity) {
				DrawEntityTree(scene->GetWorld(), rootId, editor);
			} else {
				gui.Text("No scene");
			}
		} else {
			gui.Text("No scene");
		}
		gui.EndWindow();
	}

}
