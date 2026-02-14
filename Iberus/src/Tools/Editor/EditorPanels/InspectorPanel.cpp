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

	static void DrawTransformComponent(World& world, EntityId entityId) {
		auto* transform = world.GetComponent<TransformComponent>(entityId);
		if (!transform) return;
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
			Vec3 pos = transform->Position;
			if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) transform->Position = pos;
			Vec3 rot = transform->Rotation;
			if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f)) transform->Rotation = rot;
			Vec3 scale = transform->Scale;
			if (ImGui::DragFloat3("Scale", &scale.x, 0.01f)) transform->Scale = scale;
		}
	}

	static void DrawTagComponent(World& world, EntityId entityId, Scene* scene) {
		auto* tag = world.GetComponent<TagComponent>(entityId);
		if (!tag) return;
		if (ImGui::CollapsingHeader("Tag", ImGuiTreeNodeFlags_DefaultOpen)) {
			char idBuf[256];
			snprintf(idBuf, sizeof(idBuf), "%s", tag->Id.c_str());
			if (ImGui::InputText("Id", idBuf, sizeof(idBuf))) tag->Id = idBuf;
			char nameBuf[256];
			snprintf(nameBuf, sizeof(nameBuf), "%s", tag->Name.c_str());
			if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) tag->Name = nameBuf;
		}
	}

	static void DrawActiveComponent(World& world, EntityId entityId, Scene* scene) {
		auto* active = world.GetComponent<ActiveComponent>(entityId);
		bool activeVal = active ? active->Active : true;
		if (ImGui::CollapsingHeader("Active", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Checkbox("Active", &activeVal)) {
				if (active) active->Active = activeVal;
				else scene->AddComponent<ActiveComponent>(entityId, activeVal);
			}
		}
	}

	static void DrawMeshRendererComponent(World& world, EntityId entityId) {
		auto* comp = world.GetComponent<MeshRendererComponent>(entityId);
		if (!comp) return;
		if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
			char meshBuf[256];
			snprintf(meshBuf, sizeof(meshBuf), "%s", comp->MeshId.c_str());
			if (ImGui::InputText("Mesh", meshBuf, sizeof(meshBuf))) comp->MeshId = meshBuf;
			char matBuf[256];
			snprintf(matBuf, sizeof(matBuf), "%s", comp->MaterialId.c_str());
			if (ImGui::InputText("Material", matBuf, sizeof(matBuf))) comp->MaterialId = matBuf;
		}
	}

	static void DrawCameraComponent(World& world, EntityId entityId) {
		auto* comp = world.GetComponent<CameraComponent>(entityId);
		if (!comp) return;
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
			int projType = static_cast<int>(comp->ProjectionType);
			const char* projNames[] = { "Unknown", "Orthographic", "Perspective" };
			if (ImGui::Combo("Projection", &projType, projNames, 3)) {
				comp->ProjectionType = static_cast<CameraProjectionType>(projType);
			}
			if (comp->ProjectionType == CameraProjectionType::Perspective) {
				auto& p = comp->PerspectiveParams;
				ImGui::DragFloat("FOV Y", &p.Fovy, 1.0f, 1.0f, 179.0f);
				ImGui::DragFloat("Near", &p.NearZ, 0.01f, 0.001f, 100.0f);
				ImGui::DragFloat("Far", &p.FarZ, 1.0f, 1.0f, 100000.0f);
			}
		}
	}

	static void DrawSDFComponent(World& world, EntityId entityId, Scene* scene) {
		auto* comp = world.GetComponent<SDFComponent>(entityId);
		if (!comp) return;
		if (ImGui::CollapsingHeader("SDF", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (size_t i = 0; i < comp->Parts.size(); ++i) {
				auto& part = comp->Parts[i];
				if (ImGui::TreeNode((void*)(intptr_t)i, "Part %zu", i)) {
					ImGui::DragFloat3("Position", &part.Transform.Position.x, 0.1f);
					ImGui::DragInt("Type", &part.Type);
					ImGui::DragFloat("Radius", &part.Radius, 0.01f);
					char matBuf[256];
					snprintf(matBuf, sizeof(matBuf), "%s", part.MaterialId.c_str());
					if (ImGui::InputText("Material Override", matBuf, sizeof(matBuf))) part.MaterialId = matBuf;
					ImGui::TreePop();
				}
			}
			if (ImGui::Button("Add Part")) {
				SDFPartData part;
				comp->Parts.push_back(part);
			}
		}
	}

	static void DrawLightComponent(World& world, EntityId entityId) {
		auto* comp = world.GetComponent<LightComponent>(entityId);
		if (!comp) return;
		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
			int type = static_cast<int>(comp->Type);
			const char* typeNames[] = { "Point", "Spot", "Directional", "Area" };
			if (ImGui::Combo("Type", &type, typeNames, 4)) comp->Type = static_cast<LightType>(type + 1);
			ImGui::ColorEdit3("Color", &comp->Color.x);
			ImGui::DragFloat("Intensity", &comp->Intensity, 0.1f);
			ImGui::DragFloat("Range", &comp->Range, 1.0f);
			ImGui::DragFloat3("Direction", &comp->Direction.x, 0.01f);
		}
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

		DrawTransformComponent(world, entityId);
		DrawTagComponent(world, entityId, scene);
		DrawActiveComponent(world, entityId, scene);

		if (world.HasComponent<MeshRendererComponent>(entityId)) {
			DrawMeshRendererComponent(world, entityId);
		}
		if (world.HasComponent<CameraComponent>(entityId)) {
			DrawCameraComponent(world, entityId);
		}
		if (world.HasComponent<SDFComponent>(entityId)) {
			DrawSDFComponent(world, entityId, scene);
		}
		if (world.HasComponent<LightComponent>(entityId)) {
			DrawLightComponent(world, entityId);
		}

		ImGui::Separator();
		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("AddComponentPopup");
		}
		if (ImGui::BeginPopup("AddComponentPopup")) {
			auto tryAdd = [&](const char* label, bool hasIt, auto addIt) {
				if (!hasIt && ImGui::MenuItem(label)) addIt();
			};
			tryAdd("Mesh Renderer", world.HasComponent<MeshRendererComponent>(entityId),
				[&]() { scene->AddComponent<MeshRendererComponent>(entityId); });
			tryAdd("Camera", world.HasComponent<CameraComponent>(entityId),
				[&]() { scene->AddComponent<CameraComponent>(entityId); });
			tryAdd("Light", world.HasComponent<LightComponent>(entityId),
				[&]() { scene->AddComponent<LightComponent>(entityId); });
			tryAdd("SDF", world.HasComponent<SDFComponent>(entityId),
				[&]() { scene->AddComponent<SDFComponent>(entityId); });
			ImGui::EndPopup();
		}

		gui.EndWindow();
	}

}
