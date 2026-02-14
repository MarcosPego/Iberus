#include "Enginepch.h"
#include "SceneTreePanel.h"
#include "Editor.h"
#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "World.h"
#include "Components.h"

#include "imgui.h"

namespace Iberus {

	static void DrawEntityContextMenu(Scene* scene, EntityId entityId, EntityId parentId, Editor& editor) {
		if (!scene || entityId == NullEntity || !scene->GetWorld().IsAlive(entityId)) return;
		World& world = scene->GetWorld();
		EntityId rootId = scene->GetSceneRootId();

		if (ImGui::BeginMenu("Create child")) {
			auto createChild = [&](const char* label, const char* tagBase, auto addExtraComponents) {
				if (ImGui::MenuItem(label)) {
					std::string tagId = scene->GenerateUniqueTagId(tagBase);
					EntityId childId = scene->CreateEntityECS(tagId);
					addExtraComponents(childId);
					scene->AddChildECS(entityId, childId, tagId);
					editor.SetSelectedEntity(childId);
				}
			};
			createChild("Mesh", "Mesh", [&](EntityId id) { scene->AddComponent<MeshRendererComponent>(id); });
			createChild("Camera", "Camera", [&](EntityId id) { scene->AddComponent<CameraComponent>(id); });
			createChild("Light", "Light", [&](EntityId id) { scene->AddComponent<LightComponent>(id); });
			createChild("SDF Entity", "SDFEntity", [&](EntityId id) { scene->AddComponent<SDFComponent>(id); });
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Add component")) {
			auto tryAdd = [&](const char* label, auto hasComp, auto addComp) {
				if (!hasComp() && ImGui::MenuItem(label)) addComp();
			};
			tryAdd("Mesh Renderer", [&]() { return world.HasComponent<MeshRendererComponent>(entityId); },
				[&]() { scene->AddComponent<MeshRendererComponent>(entityId); });
			tryAdd("Camera", [&]() { return world.HasComponent<CameraComponent>(entityId); },
				[&]() { scene->AddComponent<CameraComponent>(entityId); });
			tryAdd("Light", [&]() { return world.HasComponent<LightComponent>(entityId); },
				[&]() { scene->AddComponent<LightComponent>(entityId); });
			tryAdd("SDF", [&]() { return world.HasComponent<SDFComponent>(entityId); },
				[&]() { scene->AddComponent<SDFComponent>(entityId); });
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Copy")) {
			editor.SetCopiedEntity(entityId);
		}

		bool canPaste = editor.HasCopiedEntity();
		if (canPaste) {
			auto* activeScene = Engine::Instance()->GetSceneManager().GetActiveScene();
			canPaste = activeScene && activeScene->GetWorld().IsAlive(editor.GetCopiedEntityId());
		}
		if (ImGui::MenuItem("Paste", nullptr, false, canPaste)) {
			if (Scene* s = Engine::Instance()->GetSceneManager().GetActiveScene()) {
				EntityId copiedId = editor.GetCopiedEntityId();
				if (s->GetWorld().IsAlive(copiedId)) {
					EntityId cloned = s->CloneEntityWithDescendants(copiedId);
					if (cloned != NullEntity) {
						auto* tag = s->GetWorld().GetComponent<TagComponent>(cloned);
						s->AddChildECS(entityId, cloned, tag ? tag->Id : "Entity");
						editor.SetSelectedEntity(cloned);
					}
				}
			}
		}

		if (ImGui::MenuItem("Duplicate")) {
			if (Scene* s = Engine::Instance()->GetSceneManager().GetActiveScene()) {
				World& w = s->GetWorld();
				auto* hier = w.GetComponent<HierarchyComponent>(entityId);
				EntityId parId = (hier && hier->ParentId != NullEntity) ? hier->ParentId : rootId;
				if (parId != NullEntity && w.IsAlive(parId)) {
					EntityId cloned = s->CloneEntityWithDescendants(entityId);
					if (cloned != NullEntity) {
						auto* tag = w.GetComponent<TagComponent>(cloned);
						s->AddChildECS(parId, cloned, tag ? tag->Id : "Entity");
						editor.SetSelectedEntity(cloned);
					}
				}
			}
		}

		if (ImGui::MenuItem("Delete")) {
			if (entityId != rootId && scene) {
				if (editor.GetSelectedEntityId() == entityId) {
					editor.SetSelectedEntity(NullEntity);
				}
				if (editor.GetCopiedEntityId() == entityId) {
					editor.SetCopiedEntity(NullEntity);
				}
				scene->QueueDestroyEntity(entityId);
			}
		}
	}

	static void DrawEntityTree(Scene* scene, World& world, EntityId entityId, EntityId parentId, Editor& editor) {
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
		if (ImGui::BeginPopupContextItem()) {
			DrawEntityContextMenu(scene, entityId, parentId, editor);
			ImGui::EndPopup();
		}
		if (opened) {
			auto* hierarchy = world.GetComponent<HierarchyComponent>(entityId);
			if (hierarchy) {
				for (EntityId childId : hierarchy->ChildrenIds) {
					DrawEntityTree(scene, world, childId, entityId, editor);
				}
			}
			ImGui::TreePop();
		}
	}

	static void DrawEmptyContextMenu(Scene* scene, Editor& editor) {
		EntityId rootId = scene->GetSceneRootId();
		if (ImGui::BeginMenu("Create")) {
			auto createChild = [&](const char* label, const char* tagBase, auto addExtraComponents) {
				if (ImGui::MenuItem(label)) {
					std::string tagId = scene->GenerateUniqueTagId(tagBase);
					EntityId childId = scene->CreateEntityECS(tagId);
					addExtraComponents(childId);
					scene->AddChildECS(rootId, childId, tagId);
					editor.SetSelectedEntity(childId);
				}
			};
			createChild("Mesh", "Mesh", [&](EntityId id) { scene->AddComponent<MeshRendererComponent>(id); });
			createChild("Camera", "Camera", [&](EntityId id) { scene->AddComponent<CameraComponent>(id); });
			createChild("Light", "Light", [&](EntityId id) { scene->AddComponent<LightComponent>(id); });
			createChild("SDF Entity", "SDFEntity", [&](EntityId id) { scene->AddComponent<SDFComponent>(id); });
			ImGui::EndMenu();
		}
		if (editor.HasCopiedEntity()) {
			bool canPaste = false;
			if (Scene* activeScene = Engine::Instance()->GetSceneManager().GetActiveScene()) {
				canPaste = activeScene->GetWorld().IsAlive(editor.GetCopiedEntityId());
			}
			if (ImGui::MenuItem("Paste", nullptr, false, canPaste)) {
				EntityId copiedId = editor.GetCopiedEntityId();
				EntityId cloned = scene->CloneEntityWithDescendants(copiedId);
				if (cloned != NullEntity) {
					auto* tag = scene->GetWorld().GetComponent<TagComponent>(cloned);
					scene->AddChildECS(rootId, cloned, tag ? tag->Id : "Entity");
					editor.SetSelectedEntity(cloned);
				}
			}
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
				// Do not show the root entity - it is engine-internal and not user-modifiable
				auto* hierarchy = scene->GetWorld().GetComponent<HierarchyComponent>(rootId);
				if (hierarchy) {
					for (EntityId childId : hierarchy->ChildrenIds) {
						DrawEntityTree(scene, scene->GetWorld(), childId, rootId, editor);
					}
				}
				if (ImGui::BeginPopupContextWindow("SceneTreeContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
					DrawEmptyContextMenu(scene, editor);
					ImGui::EndPopup();
				}
			} else {
				gui.Text("No scene");
			}
		} else {
			gui.Text("No scene");
		}
		gui.EndWindow();
	}

}
