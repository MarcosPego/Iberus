#include "Enginepch.h"
#include "InspectorPanel.h"
#include "Editor.h"
#include "Entity.h"
#include "MathUtils.h"

#include "imgui.h"

using namespace Math;

namespace Iberus {

	InspectorPanel::InspectorPanel(Editor& editor) : editor(editor) {
	}

	void InspectorPanel::OnDraw(IGUIContext& gui) {
		if (!gui.BeginWindow("Inspector")) {
			return;
		}
		Entity* entity = editor.GetSelectedEntity();
		if (!entity) {
			gui.Text("Select an entity");
			gui.EndWindow();
			return;
		}
		char buf[256];
		snprintf(buf, sizeof(buf), "%s", entity->GetID().c_str());
		if (ImGui::InputText("ID", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly)) {
		}
		bool active = entity->GetActive();
		if (ImGui::Checkbox("Active", &active)) {
			entity->SetActive(active);
		}
		Vec3 pos = entity->GetPosition();
		if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
			entity->SetPosition(pos);
		}
		Vec3 rot = entity->GetRotation();
		if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f)) {
			entity->SetRotation(rot);
		}
		Vec3 scale = entity->GetScale();
		if (ImGui::DragFloat3("Scale", &scale.x, 0.01f)) {
			entity->SetScale(scale);
		}
		gui.EndWindow();
	}

}
