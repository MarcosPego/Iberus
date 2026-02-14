#include "Enginepch.h"
#include "InspectorPanel.h"
#include "Editor.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Components.h"
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
		EntityId entityId = editor.GetSelectedEntityId();
		if (entityId == NullEntity) {
			gui.Text("Select an entity");
			gui.EndWindow();
			return;
		}
		auto* scene = Engine::Instance()->GetSceneManager().GetActiveScene();
		if (!scene || !scene->GetWorld().IsAlive(entityId)) {
			gui.Text("Select an entity");
			gui.EndWindow();
			return;
		}
		World& world = scene->GetWorld();
		auto* tag = world.GetComponent<TagComponent>(entityId);
		char buf[256];
		snprintf(buf, sizeof(buf), "%s", tag ? tag->Id.c_str() : "?");
		ImGui::InputText("ID", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);

		auto* activeComp = world.GetComponent<ActiveComponent>(entityId);
		bool active = activeComp ? activeComp->Active : true;
		if (ImGui::Checkbox("Active", &active)) {
			if (activeComp) activeComp->Active = active;
			else scene->AddComponent<ActiveComponent>(entityId, active);
		}

		auto* transform = world.GetComponent<TransformComponent>(entityId);
		if (!transform) {
			gui.EndWindow();
			return;
		}
		Vec3 pos = transform->Position;
		if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
			transform->Position = pos;
		}
		Vec3 rot = transform->Rotation;
		if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f)) {
			transform->Rotation = rot;
		}
		Vec3 scale = transform->Scale;
		if (ImGui::DragFloat3("Scale", &scale.x, 0.01f)) {
			transform->Scale = scale;
		}
		gui.EndWindow();
	}

}
