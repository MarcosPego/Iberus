#include "Enginepch.h"
#include "SceneTreePanel.h"
#include "Editor.h"
#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "World.h"
#include "Components.h"
#include "PrefabSerializer.h"
#include "PrefabAsset.h"
#include "FileSystem.h"
#include "ResourceManager.h"
#include "FileSystemProvider.h"
#include "Mesh.h"
#include "Material.h"

#include "imgui.h"

#include <filesystem>

namespace Iberus {

	namespace {
		std::string ToForwardSlash(const std::string& s) {
			std::string r = s;
			for (char& c : r) {
				if (c == '\\') {
					c = '/';
				}
			}
			return r;
		}

		std::string GetPathRelativeToWorkingDir(const std::string& fullPath) {
			std::string work = ToForwardSlash(FileSystem::GetWorkingDir());
			std::string full = ToForwardSlash(fullPath);
			if (full.size() >= work.size() && full.compare(0, work.size(), work) == 0) {
				std::string rel = full.substr(work.size());
				if (!rel.empty() && rel[0] == '/') {
					rel = rel.substr(1);
				}
				return rel;
			}
			return full;
		}
	}

	static void DrawEntityContextMenu(Scene* scene, EntityId entityId, EntityId parentId, Editor& editor) {
		if (!scene || entityId == NullEntity || !scene->GetWorld().IsAlive(entityId)) {
			return;
		}
		World& world = scene->GetWorld();
		EntityId rootId = scene->GetSceneRootId();

		if (ImGui::BeginMenu("Create child##CreateChildMenu")) {
			auto createChild = [&](const char* label, const char* tagBase, auto addExtraComponents) {
				char buf[64];
				snprintf(buf, sizeof(buf), "%s##CreateChild_%s", label, tagBase);
				if (ImGui::MenuItem(buf)) {
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

		if (ImGui::BeginMenu("Add component##AddComponentMenu")) {
			auto tryAdd = [&](const char* label, const char* idSuffix, auto hasComp, auto addComp) {
				char buf[64];
				snprintf(buf, sizeof(buf), "%s##AddComponent_%s", label, idSuffix);
				if (!hasComp() && ImGui::MenuItem(buf)) {
					addComp();
				}
			};
			tryAdd("Mesh Renderer", "MeshRenderer", [&]() { return world.HasComponent<MeshRendererComponent>(entityId); },
				[&]() { scene->AddComponent<MeshRendererComponent>(entityId); });
			tryAdd("Camera", "Camera", [&]() { return world.HasComponent<CameraComponent>(entityId); },
				[&]() { scene->AddComponent<CameraComponent>(entityId); });
			tryAdd("Light", "Light", [&]() { return world.HasComponent<LightComponent>(entityId); },
				[&]() { scene->AddComponent<LightComponent>(entityId); });
			tryAdd("SDF", "SDF", [&]() { return world.HasComponent<SDFComponent>(entityId); },
				[&]() { scene->AddComponent<SDFComponent>(entityId); });
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Copy##EntityCopy")) {
			editor.SetCopiedEntity(entityId);
		}

		bool canPaste = editor.HasCopiedEntity();
		if (canPaste) {
			auto* activeScene = Engine::Instance()->GetSceneManager().GetActiveScene();
			canPaste = activeScene && activeScene->GetWorld().IsAlive(editor.GetCopiedEntityId());
		}
		if (ImGui::MenuItem("Paste##EntityPaste", nullptr, false, canPaste)) {
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

		if (ImGui::MenuItem("Duplicate##EntityDuplicate")) {
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

		if (ImGui::MenuItem("Delete##EntityDelete")) {
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

	static bool IsDescendantOf(World& world, EntityId entityId, EntityId potentialAncestor) {
		if (entityId == NullEntity || potentialAncestor == NullEntity) {
			return false;
		}
		EntityId current = entityId;
		while (current != NullEntity) {
			auto* hier = world.GetComponent<HierarchyComponent>(current);
			if (!hier) {
				break;
			}
			current = hier->ParentId;
			if (current == potentialAncestor) {
				return true;
			}
		}
		return false;
	}

	static void ReparentEntity(Scene* scene, EntityId entityId, EntityId newParentId, Editor& editor) {
		if (!scene || entityId == NullEntity || newParentId == NullEntity) {
			return;
		}
		World& world = scene->GetWorld();
		if (!world.IsAlive(entityId) || !world.IsAlive(newParentId)) {
			return;
		}
		if (entityId == newParentId || IsDescendantOf(world, newParentId, entityId)) {
			return;
		}
		auto* hier = world.GetComponent<HierarchyComponent>(entityId);
		EntityId oldParentId = hier ? hier->ParentId : NullEntity;
		if (oldParentId == newParentId) {
			return;
		}
		if (oldParentId != NullEntity) {
			scene->RemoveChildFromParent(oldParentId, entityId);
		}
		auto* tag = world.GetComponent<TagComponent>(entityId);
		scene->AddChildECS(newParentId, entityId, tag ? tag->Id : "Entity");
	}

	static void HandleAssetDropOnSceneTree(Scene* scene, EntityId parentId, const std::string& fullPath, Editor& editor) {
		if (!scene || parentId == NullEntity || !scene->GetWorld().IsAlive(parentId)) {
			return;
		}
		std::string ext = std::filesystem::path(fullPath).extension().string();
		if (ext == ".prefab") {
			PrefabAsset prefab = PrefabSerializer::LoadFromFile(fullPath);
			if (prefab.IsValid()) {
				EntityId instanced = PrefabSerializer::Instantiate(prefab, scene->GetWorld(), parentId);
				if (instanced != NullEntity) {
					auto* tag = scene->GetWorld().GetComponent<TagComponent>(instanced);
					editor.SetSelectedEntity(instanced);
				}
			}
		} else if (ext == ".obj" || ext == ".mesh") {
			std::string meshId = GetPathRelativeToWorkingDir(fullPath);
			if (!meshId.empty()) {
				auto* engine = Engine::Instance();
				Mesh* mesh = engine->GetResourceManager().GetOrCreateResource<Mesh>(meshId, &engine->GetEngineProvider());
				if (mesh) {
					std::string tagId = scene->GenerateUniqueTagId("Mesh");
					EntityId newId = scene->CreateEntityECS(tagId);
					auto* comp = scene->AddComponent<MeshRendererComponent>(newId);
					if (comp) {
						comp->MeshId = meshId;
						comp->MaterialId = "SDFMaterial1";
					}
					scene->AddChildECS(parentId, newId, tagId);
					editor.SetSelectedEntity(newId);
				}
			}
		}
	}

	static void DrawEntityTree(Scene* scene, World& world, EntityId entityId, EntityId parentId, Editor& editor) {
		if (entityId == NullEntity || !world.IsAlive(entityId)) {
			return;
		}
		auto* tag = world.GetComponent<TagComponent>(entityId);
		const char* name = tag ? tag->Id.c_str() : "?";
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (editor.GetSelectedEntityId() == entityId) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		ImGui::PushID(static_cast<int>(entityId));
		char treeBuf[128];
		snprintf(treeBuf, sizeof(treeBuf), "%s##Entity_%d", name, static_cast<int>(entityId));
		bool opened = ImGui::TreeNodeEx(treeBuf, flags);
		if (ImGui::IsItemClicked()) {
			editor.SetSelectedEntity(entityId);
		}
		if (ImGui::BeginPopupContextItem()) {
			DrawEntityContextMenu(scene, entityId, parentId, editor);
			ImGui::EndPopup();
		}
		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IBERUS_ASSET_PATH");
			if (payload && payload->Data) {
				std::string path(static_cast<const char*>(payload->Data));
				HandleAssetDropOnSceneTree(scene, entityId, path, editor);
			}
			payload = ImGui::AcceptDragDropPayload("IBERUS_ENTITY");
			if (payload && payload->Data && payload->DataSize >= sizeof(EntityId)) {
				EntityId draggedId = *static_cast<const EntityId*>(payload->Data);
				EntityId rootId = scene->GetSceneRootId();
				if (draggedId != NullEntity && scene->GetWorld().IsAlive(draggedId) && draggedId != rootId) {
					ReparentEntity(scene, draggedId, entityId, editor);
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
			ImGui::SetDragDropPayload("IBERUS_ENTITY", &entityId, sizeof(EntityId));
			ImGui::TextUnformatted(name);
			ImGui::EndDragDropSource();
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
		ImGui::PopID();
	}

	static void DrawEmptyContextMenu(Scene* scene, Editor& editor) {
		EntityId rootId = scene->GetSceneRootId();
		if (ImGui::BeginMenu("Create##CreateRootMenu")) {
			auto createChild = [&](const char* label, const char* tagBase, auto addExtraComponents) {
				char buf[64];
				snprintf(buf, sizeof(buf), "%s##CreateRoot_%s", label, tagBase);
				if (ImGui::MenuItem(buf)) {
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
			if (ImGui::MenuItem("Paste##EmptyPaste", nullptr, false, canPaste)) {
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
		if (!gui.BeginWindow("Scene Tree")) {
			gui.EndWindow();
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
				ImGui::Dummy(ImVec2(-1, ImGui::GetContentRegionAvail().y));
				if (ImGui::BeginDragDropTarget()) {
					const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IBERUS_ASSET_PATH");
					if (payload && payload->Data) {
						std::string path(static_cast<const char*>(payload->Data));
						HandleAssetDropOnSceneTree(scene, rootId, path, editor);
					}
					payload = ImGui::AcceptDragDropPayload("IBERUS_ENTITY");
					if (payload && payload->Data && payload->DataSize >= sizeof(EntityId)) {
						EntityId draggedId = *static_cast<const EntityId*>(payload->Data);
						if (draggedId != NullEntity && scene->GetWorld().IsAlive(draggedId) && draggedId != rootId) {
							ReparentEntity(scene, draggedId, rootId, editor);
						}
					}
					ImGui::EndDragDropTarget();
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
