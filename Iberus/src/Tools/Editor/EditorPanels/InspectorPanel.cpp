#include "Enginepch.h"
#include "InspectorPanel.h"
#include "Editor.h"
#include "Application.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Components.h"
#include "MathUtils.h"
#include "FileSystem.h"
#include "MaterialSerializer.h"
#include "ResourceManager.h"
#include "FileSystemProvider.h"
#include "Mesh.h"
#include "Material.h"

#include "imgui.h"

#include <filesystem>

using namespace Math;

namespace Iberus {

	InspectorPanel::InspectorPanel(Editor& editor) : editor(editor) {
	}

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

		bool TryResolveMeshIdFromPath(const std::string& fullPath, std::string& outMeshId) {
			std::string ext = std::filesystem::path(fullPath).extension().string();
			if (ext == ".obj" || ext == ".mesh") {
				outMeshId = GetPathRelativeToWorkingDir(fullPath);
				if (outMeshId.empty()) {
					return false;
				}
				auto* engine = Engine::Instance();
				Mesh* mesh = engine->GetResourceManager().GetOrCreateResource<Mesh>(outMeshId, &engine->GetEngineProvider());
				return mesh != nullptr;
			}
			return false;
		}

		bool TryResolveMaterialIdFromPath(const std::string& fullPath, std::string& outMaterialId) {
			std::string ext = std::filesystem::path(fullPath).extension().string();
			if (ext != ".mat") {
				return false;
			}
			std::string assetsPath = ToForwardSlash(FileSystem::GetAssetsPath());
			std::string full = ToForwardSlash(fullPath);
			std::string materialsPrefix = assetsPath + "/Materials/";
			if (full.size() >= materialsPrefix.size() && full.compare(0, materialsPrefix.size(), materialsPrefix) == 0) {
				std::string rel = full.substr(materialsPrefix.size());
				size_t sep = rel.find_last_of('/');
				if (sep != std::string::npos) {
					rel = rel.substr(sep + 1);
				}
				outMaterialId = std::filesystem::path(rel).stem().string();
				return !outMaterialId.empty();
			}
			std::string relToAssets = full;
			if (full.size() >= assetsPath.size() && full.compare(0, assetsPath.size(), assetsPath) == 0) {
				relToAssets = full.substr(assetsPath.size());
				if (!relToAssets.empty() && relToAssets[0] == '/') {
					relToAssets = relToAssets.substr(1);
				}
			}
			std::string stem = std::filesystem::path(relToAssets).stem().string();
			std::string dir = std::filesystem::path(relToAssets).parent_path().string();
			outMaterialId = dir.empty() ? stem : (ToForwardSlash(dir) + "/" + stem);
			auto* engine = Engine::Instance();
			ResourceManager& rm = engine->GetResourceManager();
			IProvider* provider = &engine->GetEngineProvider();
			auto mat = MaterialSerializer::LoadFromFile(fullPath, rm, provider);
			if (!mat) {
				return false;
			}
			rm.RegisterResource<Material>(outMaterialId, std::move(mat));
			return true;
		}

		bool TryAcceptAssetDrop(const char* slotId, bool wantMesh, bool wantMaterial, std::string* outMeshId, std::string* outMaterialId) {
			if (!ImGui::BeginDragDropTarget()) {
				return false;
			}
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IBERUS_ASSET_PATH");
			ImGui::EndDragDropTarget();
			if (!payload || !payload->Data) {
				return false;
			}
			const char* pathStr = static_cast<const char*>(payload->Data);
			std::string path(pathStr);
			if (path.empty()) {
				return false;
			}
			std::string meshId, materialId;
			if (wantMesh && TryResolveMeshIdFromPath(path, meshId)) {
				if (outMeshId) {
					*outMeshId = meshId;
				}
				return true;
			}
			if (wantMaterial && TryResolveMaterialIdFromPath(path, materialId)) {
				if (outMaterialId) {
					*outMaterialId = materialId;
				}
				return true;
			}
			return false;
		}
	}

	static void DrawTransformComponent(World& world, EntityId entityId) {
		auto* transform = world.GetComponent<TransformComponent>(entityId);
		if (!transform) {
			return;
		}
		if (ImGui::CollapsingHeader("Transform##TransformHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
			Vec3 pos = transform->Position;
			if (ImGui::DragFloat3("Position##TransformPosition", &pos.x, 0.1f)) {
				transform->Position = pos;
			}
			Vec3 rot = transform->Rotation;
			if (ImGui::DragFloat3("Rotation##TransformRotation", &rot.x, 1.0f)) {
				transform->Rotation = rot;
			}
			Vec3 scale = transform->Scale;
			if (ImGui::DragFloat3("Scale##TransformScale", &scale.x, 0.01f)) {
				transform->Scale = scale;
			}
		}
	}

	static void DrawTagComponent(World& world, EntityId entityId, Scene* scene) {
		auto* tag = world.GetComponent<TagComponent>(entityId);
		if (!tag) {
			return;
		}
		if (ImGui::CollapsingHeader("Tag##TagHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
			char idBuf[256];
			snprintf(idBuf, sizeof(idBuf), "%s", tag->Id.c_str());
			if (ImGui::InputText("Id##TagId", idBuf, sizeof(idBuf))) {
				tag->Id = idBuf;
			}
			char nameBuf[256];
			snprintf(nameBuf, sizeof(nameBuf), "%s", tag->Name.c_str());
			if (ImGui::InputText("Name##TagName", nameBuf, sizeof(nameBuf))) {
				tag->Name = nameBuf;
			}
		}
	}

	static void DrawActiveComponent(World& world, EntityId entityId, Scene* scene) {
		auto* active = world.GetComponent<ActiveComponent>(entityId);
		bool activeVal = active ? active->Active : true;
		if (ImGui::CollapsingHeader("Active##ActiveHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Checkbox("Active##ActiveCheckbox", &activeVal)) {
				if (active) {
					active->Active = activeVal;
				} else {
					scene->AddComponent<ActiveComponent>(entityId, activeVal);
				}
			}
		}
	}

	static void DrawMeshRendererComponent(World& world, EntityId entityId) {
		auto* comp = world.GetComponent<MeshRendererComponent>(entityId);
		if (!comp) {
			return;
		}
		if (ImGui::CollapsingHeader("Mesh Renderer##MeshRendererHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
			char meshBuf[256];
			snprintf(meshBuf, sizeof(meshBuf), "%s", comp->MeshId.c_str());
			if (ImGui::InputText("Mesh##MeshRendererMesh", meshBuf, sizeof(meshBuf))) {
				comp->MeshId = meshBuf;
			}
			std::string dropMeshId;
			if (TryAcceptAssetDrop("##MeshRendererMeshDrop", true, false, &dropMeshId, nullptr)) {
				comp->MeshId = dropMeshId;
			}

			char matBuf[256];
			snprintf(matBuf, sizeof(matBuf), "%s", comp->MaterialId.c_str());
			if (ImGui::InputText("Material##MeshRendererMaterial", matBuf, sizeof(matBuf))) {
				comp->MaterialId = matBuf;
			}
			std::string dropMatId;
			if (TryAcceptAssetDrop("##MeshRendererMaterialDrop", false, true, nullptr, &dropMatId)) {
				comp->MaterialId = dropMatId;
			}
		}
	}

	static void DrawCameraComponent(World& world, EntityId entityId) {
		auto* comp = world.GetComponent<CameraComponent>(entityId);
		if (!comp) {
			return;
		}
		if (ImGui::CollapsingHeader("Camera##CameraHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
			int projType = static_cast<int>(comp->ProjectionType);
			const char* projNames[] = { "Unknown", "Orthographic", "Perspective" };
			if (ImGui::Combo("Projection##CameraProjection", &projType, projNames, 3)) {
				comp->ProjectionType = static_cast<CameraProjectionType>(projType);
			}
			if (comp->ProjectionType == CameraProjectionType::Perspective) {
				auto& p = comp->PerspectiveParams;
				ImGui::DragFloat("FOV Y##CameraFov", &p.Fovy, 1.0f, 1.0f, 179.0f);
				ImGui::DragFloat("Near##CameraNear", &p.NearZ, 0.01f, 0.001f, 100.0f);
				ImGui::DragFloat("Far##CameraFar", &p.FarZ, 1.0f, 1.0f, 100000.0f);
			}
		}
	}

	static void DrawSDFComponent(World& world, EntityId entityId, Scene* scene) {
		auto* comp = world.GetComponent<SDFComponent>(entityId);
		if (!comp) {
			return;
		}
		if (ImGui::CollapsingHeader("SDF##SDFHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("Part count: %zu", comp->Parts.size());

			for (size_t i = 0; i < comp->Parts.size(); ++i) {
				auto& part = comp->Parts[i];
				ImGui::PushID(static_cast<int>(i));

				if (ImGui::Button("Up##SDFPartUp")) {
					if (i > 0) {
						std::swap(comp->Parts[i], comp->Parts[i - 1]);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Down##SDFPartDown")) {
					if (i + 1 < comp->Parts.size()) {
						std::swap(comp->Parts[i], comp->Parts[i + 1]);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove##SDFPartRemove")) {
					comp->Parts.erase(comp->Parts.begin() + static_cast<std::ptrdiff_t>(i));
					ImGui::PopID();
					break;
				}
				ImGui::SameLine();

				if (ImGui::TreeNode((void*)(intptr_t)i, "Part %zu##SDFPart", i)) {
					ImGui::DragFloat3("Position##SDFPartPosition", &part.Transform.Position.x, 0.1f);

					int typeIdx = (part.Type == 1) ? 0 : (part.Type == 2) ? 1 : (part.Type == 3) ? 2 : 0;
					const char* typeNames[] = { "Sphere", "Box", "Capsule" };
					if (ImGui::Combo("Type##SDFPartType", &typeIdx, typeNames, 3)) {
						part.Type = typeIdx + 1;
					}

					ImGui::DragFloat("Radius##SDFPartRadius", &part.Radius, 0.01f);

					if (part.Type == 3) {
						ImGui::DragFloat3("Endpoint##SDFPartEndpoint", &part.Endpoint.x, 0.1f);
					}

					char matBuf[256];
					snprintf(matBuf, sizeof(matBuf), "%s", part.MaterialId.c_str());
					if (ImGui::InputText("Material Override##SDFPartMaterial", matBuf, sizeof(matBuf))) {
						part.MaterialId = matBuf;
					}
					std::string dropPartMatId;
					if (TryAcceptAssetDrop("##SDFPartMaterialDrop", false, true, nullptr, &dropPartMatId)) {
						part.MaterialId = dropPartMatId;
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}

			if (ImGui::BeginMenu("Add Part##SDFAddPartMenu")) {
				if (ImGui::MenuItem("Sphere##AddSphere")) {
					SDFPartData part;
					part.Type = 1;
					part.Radius = 0.5f;
					part.MaterialId = "SDFMaterial1";
					comp->Parts.push_back(part);
				}
				if (ImGui::MenuItem("Box##AddBox")) {
					SDFPartData part;
					part.Type = 2;
					part.Radius = 0.5f;
					part.MaterialId = "SDFMaterial1";
					comp->Parts.push_back(part);
				}
				if (ImGui::MenuItem("Capsule##AddCapsule")) {
					SDFPartData part;
					part.Type = 3;
					part.Transform.Position = Vec3(-0.5f, 0, 0);
					part.Endpoint = Vec3(0.5f, 0, 0);
					part.Radius = 0.25f;
					part.MaterialId = "SDFMaterial1";
					comp->Parts.push_back(part);
				}
				ImGui::EndMenu();
			}
		}
	}

	static void DrawLightComponent(World& world, EntityId entityId) {
		auto* comp = world.GetComponent<LightComponent>(entityId);
		if (!comp) {
			return;
		}
		if (ImGui::CollapsingHeader("Light##LightHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
			// LightType is 1-based (Point=1, Spot=2, etc); Combo uses 0-based indices
			int type = static_cast<int>(comp->Type) - 1;
			type = (type < 0) ? 0 : (type > 3 ? 3 : type);
			const char* typeNames[] = { "Point", "Spot", "Directional", "Area" };
			if (ImGui::Combo("Type##LightType", &type, typeNames, 4)) {
				comp->Type = static_cast<LightType>(type + 1);
			}
			ImGui::ColorEdit3("Color##LightColor", &comp->Color.x);
			ImGui::DragFloat("Intensity##LightIntensity", &comp->Intensity, 0.1f);
			ImGui::DragFloat("Range##LightRange", &comp->Range, 1.0f);
			ImGui::DragFloat3("Direction##LightDirection", &comp->Direction.x, 0.01f);
		}
	}

	static void DrawScriptComponent(World& world, EntityId entityId) {
		auto* comp = world.GetComponent<ScriptComponent>(entityId);
		if (!comp) {
			return;
		}
		if (ImGui::CollapsingHeader("Script##ScriptHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
			char assemBuf[256];
			snprintf(assemBuf, sizeof(assemBuf), "%s", comp->AssemblyPath.c_str());
			if (ImGui::InputText("Assembly##ScriptAssembly", assemBuf, sizeof(assemBuf))) {
				comp->AssemblyPath = assemBuf;
			}
			char typeBuf[256];
			snprintf(typeBuf, sizeof(typeBuf), "%s", comp->TypeName.c_str());
			if (ImGui::InputText("Type##ScriptType", typeBuf, sizeof(typeBuf))) {
				comp->TypeName = typeBuf;
			}
		}
	}

	static void DrawTerrainComponent(World& world, EntityId entityId) {
		auto* comp = world.GetComponent<TerrainComponent>(entityId);
		if (!comp) {
			return;
		}
		if (ImGui::CollapsingHeader("Terrain##TerrainHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
			auto setDirty = [comp]() { comp->NeedsRegenerate = true; };
			if (ImGui::DragInt("Width##TerrainWidth", &comp->Width, 1.0f, 2, 512)) {
				setDirty();
			}
			if (ImGui::DragInt("Height##TerrainHeight", &comp->Height, 1.0f, 2, 512)) {
				setDirty();
			}
			if (ImGui::DragFloat("Height Scale##TerrainHeightScale", &comp->HeightScale, 0.5f, 0.0f, 100.0f)) {
				setDirty();
			}
			if (ImGui::DragFloat("Frequency##TerrainFrequency", &comp->Frequency, 0.001f, 0.001f, 1.0f)) {
				setDirty();
			}
			if (ImGui::DragInt("Seed##TerrainSeed", &comp->Seed)) {
				setDirty();
			}
			if (ImGui::DragInt("Octaves##TerrainOctaves", &comp->Octaves, 1.0f, 1, 8)) {
				setDirty();
			}
			if (ImGui::DragFloat("X Offset##TerrainXOffset", &comp->XOffset, 0.1f)) {
				setDirty();
			}
			if (ImGui::DragFloat("Z Offset##TerrainZOffset", &comp->ZOffset, 0.1f)) {
				setDirty();
			}
			if (ImGui::DragFloat("World Size X##TerrainWorldSizeX", &comp->WorldSizeX, 1.0f, 1.0f, 1000.0f)) {
				setDirty();
			}
			if (ImGui::DragFloat("World Size Z##TerrainWorldSizeZ", &comp->WorldSizeZ, 1.0f, 1.0f, 1000.0f)) {
				setDirty();
			}
			char meshBuf[256];
			snprintf(meshBuf, sizeof(meshBuf), "%s", comp->MeshId.c_str());
			if (ImGui::InputText("Mesh Id##TerrainMeshId", meshBuf, sizeof(meshBuf))) {
				comp->MeshId = meshBuf;
				setDirty();
			}
			std::string dropTerrainMeshId;
			if (TryAcceptAssetDrop("##TerrainMeshDrop", true, false, &dropTerrainMeshId, nullptr)) {
				comp->MeshId = dropTerrainMeshId;
				setDirty();
			}
			char matBuf[256];
			snprintf(matBuf, sizeof(matBuf), "%s", comp->MaterialId.c_str());
			if (ImGui::InputText("Material Id##TerrainMaterialId", matBuf, sizeof(matBuf))) {
				comp->MaterialId = matBuf;
				setDirty();
			}
			std::string dropTerrainMatId;
			if (TryAcceptAssetDrop("##TerrainMaterialDrop", false, true, nullptr, &dropTerrainMatId)) {
				comp->MaterialId = dropTerrainMatId;
				setDirty();
			}
			if (ImGui::Button("Regenerate##TerrainRegenerate")) {
				comp->NeedsRegenerate = true;
			}
		}
	}

	void InspectorPanel::OnDraw(IGUIContext& gui, bool* p_open) {
		if (!gui.BeginWindow("Inspector", p_open)) {
			gui.EndWindow();
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
		if (world.HasComponent<TerrainComponent>(entityId)) {
			DrawTerrainComponent(world, entityId);
		}
		if (world.HasComponent<ScriptComponent>(entityId)) {
			DrawScriptComponent(world, entityId);
		}

		ImGui::Separator();
		if (ImGui::Button("Add Component##AddComponent")) {
			ImGui::OpenPopup("AddComponentPopup");
		}
		if (ImGui::BeginPopup("AddComponentPopup")) {
			auto tryAdd = [&](const char* label, const char* idSuffix, bool hasIt, auto addIt) {
				char buf[64];
				snprintf(buf, sizeof(buf), "%s##%s", label, idSuffix);
				if (!hasIt && ImGui::MenuItem(buf)) {
					addIt();
				}
			};
			tryAdd("Mesh Renderer", "AddMeshRenderer", world.HasComponent<MeshRendererComponent>(entityId),
				[&]() { scene->AddComponent<MeshRendererComponent>(entityId); });
			tryAdd("Camera", "AddCamera", world.HasComponent<CameraComponent>(entityId),
				[&]() { scene->AddComponent<CameraComponent>(entityId); });
			tryAdd("Light", "AddLight", world.HasComponent<LightComponent>(entityId),
				[&]() { scene->AddComponent<LightComponent>(entityId); });
			tryAdd("SDF", "AddSDF", world.HasComponent<SDFComponent>(entityId),
				[&]() { scene->AddComponent<SDFComponent>(entityId); });
			tryAdd("Terrain", "AddTerrain", world.HasComponent<TerrainComponent>(entityId),
				[&]() { scene->AddComponent<TerrainComponent>(entityId); });
			tryAdd("Script", "AddScript", world.HasComponent<ScriptComponent>(entityId),
				[&]() {
					auto* c = scene->AddComponent<ScriptComponent>(entityId);
					if (c) {
						auto* project = Iberus::Application::Get()->GetProject();
						if (project && !project->GetName().empty()) {
							c->AssemblyPath = "Assets/Scripts/" + project->GetName() + ".Scripts.dll";
						} else {
							c->AssemblyPath = "";
						}
						c->TypeName = "";
					}
				});
			ImGui::EndPopup();
		}

		gui.EndWindow();
	}

}
