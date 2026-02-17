#include "Enginepch.h"
#include "SceneSerializer.h"
#include "Scene.h"
#include "World.h"
#include "Components.h"
#include "FileSystem.h"

#include <queue>

namespace Iberus {

	using namespace Math;

	namespace {

		void SerializeVec3(JsonValue& obj, const char* key, const Vec3& v) {
			JsonValue arr = JsonValue::Array();
			arr.Set(0, JsonValue::Number(v.x));
			arr.Set(1, JsonValue::Number(v.y));
			arr.Set(2, JsonValue::Number(v.z));
			obj.Set(key, arr);
		}

		void SerializeVec4(JsonValue& obj, const char* key, const Vec4& v) {
			JsonValue arr = JsonValue::Array();
			arr.Set(0, JsonValue::Number(v.x));
			arr.Set(1, JsonValue::Number(v.y));
			arr.Set(2, JsonValue::Number(v.z));
			arr.Set(3, JsonValue::Number(v.w));
			obj.Set(key, arr);
		}

		Vec3 DeserializeVec3(const JsonValue& obj, const char* key) {
			JsonValue arr = obj[key];
			if (!arr.IsArray() || arr.Size() < 3) {
				return Vec3(0, 0, 0);
			}
			return Vec3(arr[0].AsFloat(), arr[1].AsFloat(), arr[2].AsFloat());
		}

		Vec4 DeserializeVec4(const JsonValue& obj, const char* key) {
			JsonValue arr = obj[key];
			if (!arr.IsArray() || arr.Size() < 4) {
				return Vec4(1, 1, 1, 1);
			}
			return Vec4(arr[0].AsFloat(), arr[1].AsFloat(), arr[2].AsFloat(), arr[3].AsFloat());
		}

		void SerializeEntity(const World& world, EntityId entityId, const std::string& tempId,
			const std::string& parentTempId, const std::vector<std::string>& childrenTempIds,
			JsonValue& outEntity) {
			outEntity.Set("id", tempId);
			if (!parentTempId.empty()) {
				outEntity.Set("parent", parentTempId);
			}

			if (auto* tag = world.GetComponent<TagComponent>(entityId)) {
				JsonValue tagObj = JsonValue::Object();
				tagObj.Set("Id", tag->Id);
				tagObj.Set("Name", tag->Name);
				outEntity.Set("tag", tagObj);
			}

			if (auto* t = world.GetComponent<TransformComponent>(entityId)) {
				JsonValue trObj = JsonValue::Object();
				SerializeVec3(trObj, "Position", t->Position);
				SerializeVec3(trObj, "Rotation", t->Rotation);
				SerializeVec3(trObj, "Scale", t->Scale);
				outEntity.Set("transform", trObj);
			}

			if (!childrenTempIds.empty()) {
				JsonValue childrenJson = JsonValue::Array();
				for (const auto& c : childrenTempIds) {
					childrenJson.Push(JsonValue::String(c));
				}
				outEntity.Set("children", childrenJson);
			}

			if (auto* a = world.GetComponent<ActiveComponent>(entityId)) {
				outEntity.Set("active", a->Active);
			}

			if (auto* mr = world.GetComponent<MeshRendererComponent>(entityId)) {
				JsonValue mrObj = JsonValue::Object();
				mrObj.Set("MeshId", mr->MeshId);
				mrObj.Set("MaterialId", mr->MaterialId);
				outEntity.Set("meshRenderer", mrObj);
			}

			if (auto* cam = world.GetComponent<CameraComponent>(entityId)) {
				JsonValue camObj = JsonValue::Object();
				camObj.Set("ProjectionType", static_cast<int>(cam->ProjectionType));
				camObj.Set("Fovy", cam->PerspectiveParams.Fovy);
				camObj.Set("Aspect", cam->PerspectiveParams.Aspect);
				camObj.Set("NearZ", cam->PerspectiveParams.NearZ);
				camObj.Set("FarZ", cam->PerspectiveParams.FarZ);
				camObj.Set("OrthoLeft", cam->OrthoParams.Left);
				camObj.Set("OrthoRight", cam->OrthoParams.Right);
				camObj.Set("OrthoBottom", cam->OrthoParams.Bottom);
				camObj.Set("OrthoTop", cam->OrthoParams.Top);
				camObj.Set("OrthoNear", cam->OrthoParams.Near);
				camObj.Set("OrthoFar", cam->OrthoParams.Far);
				outEntity.Set("camera", camObj);
			}

			if (auto* light = world.GetComponent<LightComponent>(entityId)) {
				JsonValue lightObj = JsonValue::Object();
				lightObj.Set("Type", static_cast<int>(light->Type));
				SerializeVec3(lightObj, "Color", light->Color);
				lightObj.Set("Intensity", light->Intensity);
				lightObj.Set("Constant", light->Constant);
				lightObj.Set("Linear", light->Linear);
				lightObj.Set("Quadratic", light->Quadratic);
				lightObj.Set("Range", light->Range);
				lightObj.Set("SpotAngle", light->SpotAngle);
				SerializeVec3(lightObj, "Direction", light->Direction);
				outEntity.Set("light", lightObj);
			}

			if (auto* sdf = world.GetComponent<SDFComponent>(entityId)) {
				JsonValue partsArr = JsonValue::Array();
				for (size_t i = 0; i < sdf->Parts.size(); ++i) {
					const auto& p = sdf->Parts[i];
					JsonValue partObj = JsonValue::Object();
					JsonValue pt = JsonValue::Object();
					SerializeVec3(pt, "Position", p.Transform.Position);
					SerializeVec3(pt, "Rotation", p.Transform.Rotation);
					SerializeVec3(pt, "Scale", p.Transform.Scale);
					partObj.Set("transform", pt);
					partObj.Set("Type", p.Type);
					SerializeVec3(partObj, "Endpoint", p.Endpoint);
					partObj.Set("Radius", p.Radius);
				partObj.Set("MaterialId", p.MaterialId);
				partsArr.Push(partObj);
			}
			outEntity.Set("sdf", partsArr);
			}

			if (auto* terrain = world.GetComponent<TerrainComponent>(entityId)) {
				JsonValue tObj = JsonValue::Object();
				tObj.Set("Width", terrain->Width);
				tObj.Set("Height", terrain->Height);
				tObj.Set("XOffset", terrain->XOffset);
				tObj.Set("ZOffset", terrain->ZOffset);
				tObj.Set("Frequency", terrain->Frequency);
				tObj.Set("Seed", terrain->Seed);
				tObj.Set("Octaves", terrain->Octaves);
				tObj.Set("HeightScale", terrain->HeightScale);
				tObj.Set("WorldSizeX", terrain->WorldSizeX);
				tObj.Set("WorldSizeZ", terrain->WorldSizeZ);
				tObj.Set("MeshId", terrain->MeshId);
				tObj.Set("MaterialId", terrain->MaterialId);
				tObj.Set("NeedsRegenerate", terrain->NeedsRegenerate);
				outEntity.Set("terrain", tObj);
			}
		}

		void CollectEntitiesRecursive(const World& world, EntityId entityId,
			std::vector<EntityId>& order,
			std::unordered_map<EntityId, std::string>& entityToTempId,
			int& nextTemp) {
			std::string tempId = "ent_" + std::to_string(nextTemp++);
			entityToTempId[entityId] = tempId;
			order.push_back(entityId);

			auto* hierarchy = world.GetComponent<HierarchyComponent>(entityId);
			if (hierarchy) {
				for (EntityId childId : hierarchy->ChildrenIds) {
					if (world.IsAlive(childId)) {
						CollectEntitiesRecursive(world, childId, order, entityToTempId, nextTemp);
					}
				}
			}
		}

	} // namespace

	Buffer SceneSerializer::Serialize(const World& world, EntityId sceneRootId, EntityId activeCameraId, EntityId rootId) {
		EntityId serializeRoot = (rootId != NullEntity) ? rootId : sceneRootId;
		if (!world.IsAlive(serializeRoot)) {
			return Buffer();
		}

		std::vector<EntityId> order;
		std::unordered_map<EntityId, std::string> entityToTempId;
		int nextTemp = 0;
		CollectEntitiesRecursive(world, serializeRoot, order, entityToTempId, nextTemp);

		std::string activeCameraTempId;
		if (world.IsAlive(activeCameraId) && entityToTempId.count(activeCameraId)) {
			activeCameraTempId = entityToTempId[activeCameraId];
		}

		JsonValue root = JsonValue::Object();
		root.Set("version", 1);
		root.Set("activeCameraId", activeCameraTempId);

		JsonValue entitiesArr = JsonValue::Array();
		for (EntityId entityId : order) {
			std::string tempId = entityToTempId[entityId];
			std::string parentTempId;
			std::vector<std::string> childrenTempIds;

			auto* hierarchy = world.GetComponent<HierarchyComponent>(entityId);
			if (hierarchy) {
				if (hierarchy->ParentId != NullEntity && entityToTempId.count(hierarchy->ParentId)) {
					parentTempId = entityToTempId[hierarchy->ParentId];
				}
				for (EntityId childId : hierarchy->ChildrenIds) {
					if (entityToTempId.count(childId)) {
						childrenTempIds.push_back(entityToTempId[childId]);
					}
				}
			}

			JsonValue entityJson = JsonValue::Object();
			SerializeEntity(world, entityId, tempId, parentTempId, childrenTempIds, entityJson);

			entitiesArr.Push(entityJson);
		}
		root.Set("entities", entitiesArr);

		return root.ToBuffer(true);
	}

	EntityId SceneSerializer::Deserialize(World& world, const Buffer& buffer, EntityId parentId, EntityId& outActiveCameraId) {
		JsonValue root = JsonValue::Parse(buffer);
		if (!root.IsValid() || !root.IsObject()) {
			outActiveCameraId = NullEntity;
			return NullEntity;
		}

		std::string activeCameraTempId = root.Contains("activeCameraId") ? root["activeCameraId"].AsString() : std::string();

		JsonValue entitiesArr = root["entities"];
		if (!entitiesArr.IsArray()) {
			outActiveCameraId = NullEntity;
			return NullEntity;
		}

		std::unordered_map<std::string, EntityId> tempIdToEntity;
		EntityId firstRootId = NullEntity;

		for (size_t i = 0; i < entitiesArr.Size(); ++i) {
			JsonValue entityJson = entitiesArr[i];
			if (!entityJson.IsObject()) {
				continue;
			}

			std::string tempId = entityJson.Contains("id") ? entityJson["id"].AsString() : ("ent_" + std::to_string(i));
			std::string parentTempId = entityJson.Contains("parent") ? entityJson["parent"].AsString() : std::string();

			std::string tagId = "Entity";
			std::string tagName = "Entity";
			if (entityJson.Contains("tag")) {
				JsonValue tagObj = entityJson["tag"];
				if (tagObj.Contains("Id")) {
					tagId = tagObj["Id"].AsString();
				}
				if (tagObj.Contains("Name")) {
					tagName = tagObj["Name"].AsString();
				}
			}

			EntityId entityId = world.CreateEntity();
			world.AddComponent<TagComponent>(entityId, tagId, tagName);
			world.AddComponent<TransformComponent>(entityId);
			world.AddComponent<HierarchyComponent>(entityId);
			world.AddComponent<ActiveComponent>(entityId, true);

			tempIdToEntity[tempId] = entityId;
			if (firstRootId == NullEntity && parentTempId.empty()) {
				firstRootId = entityId;
			}

			if (entityJson.Contains("transform")) {
				JsonValue tr = entityJson["transform"];
				if (auto* t = world.GetComponent<TransformComponent>(entityId)) {
					t->Position = DeserializeVec3(tr, "Position");
					t->Rotation = DeserializeVec3(tr, "Rotation");
					t->Scale = DeserializeVec3(tr, "Scale");
				}
			}

			if (entityJson.Contains("active")) {
				if (auto* a = world.GetComponent<ActiveComponent>(entityId)) {
					a->Active = entityJson["active"].AsBool();
				}
			}

			if (entityJson.Contains("meshRenderer")) {
				JsonValue mr = entityJson["meshRenderer"];
				auto* comp = world.AddComponent<MeshRendererComponent>(entityId);
				if (comp) {
					comp->MeshId = mr.Contains("MeshId") ? mr["MeshId"].AsString() : std::string();
					comp->MaterialId = mr.Contains("MaterialId") ? mr["MaterialId"].AsString() : std::string();
				}
			}

			if (entityJson.Contains("camera")) {
				JsonValue cam = entityJson["camera"];
				auto* comp = world.AddComponent<CameraComponent>(entityId);
				if (comp) {
					comp->ProjectionType = static_cast<CameraProjectionType>(cam.Contains("ProjectionType") ? cam["ProjectionType"].AsInt() : 0);
					if (cam.Contains("Fovy")) {
						comp->PerspectiveParams.Fovy = cam["Fovy"].AsFloat();
					}
					if (cam.Contains("Aspect")) {
						comp->PerspectiveParams.Aspect = cam["Aspect"].AsFloat();
					}
					if (cam.Contains("NearZ")) {
						comp->PerspectiveParams.NearZ = cam["NearZ"].AsFloat();
					}
					if (cam.Contains("FarZ")) {
						comp->PerspectiveParams.FarZ = cam["FarZ"].AsFloat();
					}
					if (cam.Contains("OrthoLeft")) {
						comp->OrthoParams.Left = cam["OrthoLeft"].AsFloat();
					}
					if (cam.Contains("OrthoRight")) {
						comp->OrthoParams.Right = cam["OrthoRight"].AsFloat();
					}
					if (cam.Contains("OrthoBottom")) {
						comp->OrthoParams.Bottom = cam["OrthoBottom"].AsFloat();
					}
					if (cam.Contains("OrthoTop")) {
						comp->OrthoParams.Top = cam["OrthoTop"].AsFloat();
					}
					if (cam.Contains("OrthoNear")) {
						comp->OrthoParams.Near = cam["OrthoNear"].AsFloat();
					}
					if (cam.Contains("OrthoFar")) {
						comp->OrthoParams.Far = cam["OrthoFar"].AsFloat();
					}
				}
			}

			if (entityJson.Contains("light")) {
				JsonValue light = entityJson["light"];
				auto* comp = world.AddComponent<LightComponent>(entityId);
				if (comp) {
					comp->Type = static_cast<LightType>(light.Contains("Type") ? light["Type"].AsInt() : 1);
					comp->Color = DeserializeVec3(light, "Color");
					comp->Intensity = light.Contains("Intensity") ? light["Intensity"].AsFloat() : 1.0f;
					comp->Constant = light.Contains("Constant") ? light["Constant"].AsFloat() : 1.0f;
					comp->Linear = light.Contains("Linear") ? light["Linear"].AsFloat() : 0.09f;
					comp->Quadratic = light.Contains("Quadratic") ? light["Quadratic"].AsFloat() : 0.032f;
					comp->Range = light.Contains("Range") ? light["Range"].AsFloat() : 50.0f;
					comp->SpotAngle = light.Contains("SpotAngle") ? light["SpotAngle"].AsFloat() : 45.0f;
					comp->Direction = DeserializeVec3(light, "Direction");
				}
			}

			if (entityJson.Contains("sdf")) {
				JsonValue partsArr = entityJson["sdf"];
				auto* comp = world.AddComponent<SDFComponent>(entityId);
				if (comp && partsArr.IsArray()) {
					for (size_t pi = 0; pi < partsArr.Size(); ++pi) {
						JsonValue partJson = partsArr[pi];
						if (!partJson.IsObject()) {
							continue;
						}
						SDFPartData part;
						if (partJson.Contains("transform")) {
							JsonValue pt = partJson["transform"];
							part.Transform.Position = DeserializeVec3(pt, "Position");
							part.Transform.Rotation = DeserializeVec3(pt, "Rotation");
							part.Transform.Scale = DeserializeVec3(pt, "Scale");
						}
						part.Type = partJson.Contains("Type") ? partJson["Type"].AsInt() : 1;
						part.Endpoint = DeserializeVec3(partJson, "Endpoint");
						part.Radius = partJson.Contains("Radius") ? partJson["Radius"].AsFloat() : 1.0f;
						part.MaterialId = partJson.Contains("MaterialId") ? partJson["MaterialId"].AsString() : std::string();
						comp->Parts.push_back(part);
					}
				}
			}

			if (entityJson.Contains("terrain")) {
				JsonValue terrain = entityJson["terrain"];
				auto* comp = world.AddComponent<TerrainComponent>(entityId);
				if (comp) {
					comp->Width = terrain.Contains("Width") ? terrain["Width"].AsInt() : 65;
					comp->Height = terrain.Contains("Height") ? terrain["Height"].AsInt() : 65;
					comp->XOffset = terrain.Contains("XOffset") ? terrain["XOffset"].AsFloat() : 0.0f;
					comp->ZOffset = terrain.Contains("ZOffset") ? terrain["ZOffset"].AsFloat() : 0.0f;
					comp->Frequency = terrain.Contains("Frequency") ? terrain["Frequency"].AsFloat() : 0.01f;
					comp->Seed = terrain.Contains("Seed") ? terrain["Seed"].AsInt() : 1337;
					comp->Octaves = terrain.Contains("Octaves") ? terrain["Octaves"].AsInt() : 2;
					comp->HeightScale = terrain.Contains("HeightScale") ? terrain["HeightScale"].AsFloat() : 25.0f;
					comp->WorldSizeX = terrain.Contains("WorldSizeX") ? terrain["WorldSizeX"].AsFloat() : 60.0f;
					comp->WorldSizeZ = terrain.Contains("WorldSizeZ") ? terrain["WorldSizeZ"].AsFloat() : 60.0f;
					comp->MeshId = terrain.Contains("MeshId") ? terrain["MeshId"].AsString() : std::string("terrain_heightmap");
					comp->MaterialId = terrain.Contains("MaterialId") ? terrain["MaterialId"].AsString() : std::string("PlaneMaterial");
					comp->NeedsRegenerate = terrain.Contains("NeedsRegenerate") ? terrain["NeedsRegenerate"].AsBool() : true;
				}
			}
		}

		for (size_t i = 0; i < entitiesArr.Size(); ++i) {
			JsonValue entityJson = entitiesArr[i];
			if (!entityJson.IsObject()) {
				continue;
			}
			std::string tempId = entityJson.Contains("id") ? entityJson["id"].AsString() : ("ent_" + std::to_string(i));
			std::string parentTempId = entityJson.Contains("parent") ? entityJson["parent"].AsString() : std::string();

			auto it = tempIdToEntity.find(tempId);
			if (it == tempIdToEntity.end()) {
				continue;
			}
			EntityId entityId = it->second;

			EntityId resolvedParent = parentId;
			if (!parentTempId.empty()) {
				auto pit = tempIdToEntity.find(parentTempId);
				if (pit != tempIdToEntity.end()) {
					resolvedParent = pit->second;
				}
			}

			if (resolvedParent != NullEntity) {
				auto* childHierarchy = world.GetComponent<HierarchyComponent>(entityId);
				auto* parentHierarchy = world.GetComponent<HierarchyComponent>(resolvedParent);
				if (childHierarchy && parentHierarchy) {
					childHierarchy->ParentId = resolvedParent;
					parentHierarchy->ChildrenIds.push_back(entityId);
				}
			}
		}

		if (!activeCameraTempId.empty()) {
			auto ait = tempIdToEntity.find(activeCameraTempId);
			if (ait != tempIdToEntity.end()) {
				outActiveCameraId = ait->second;
			} else {
				outActiveCameraId = NullEntity;
			}
		} else {
			outActiveCameraId = NullEntity;
		}

		return firstRootId != NullEntity ? firstRootId : (tempIdToEntity.empty() ? NullEntity : tempIdToEntity.begin()->second);
	}

	bool SceneSerializer::SaveToFile(const Scene& scene, const std::string& path) {
		Buffer buf = Serialize(scene.GetWorld(), scene.GetSceneRootId(), scene.GetActiveCameraId(), NullEntity);
		if (buf.Invalid()) {
			return false;
		}
		std::ofstream f(path, std::ios::binary);
		if (!f) {
			return false;
		}
		f.write(reinterpret_cast<const char*>(buf.GetData()), static_cast<std::streamsize>(buf.GetSize()));
		return f.good();
	}

	bool SceneSerializer::LoadFromFile(Scene& scene, const std::string& path) {
		Buffer buf = FileSystem::GetRawFileBuffer(path);
		if (buf.Invalid()) {
			return false;
		}

		EntityId oldRoot = scene.GetSceneRootId();
		scene.DestroyEntityWithDescendants(oldRoot);

		World& world = scene.GetWorld();
		EntityId newActiveCamera = NullEntity;
		EntityId newRoot = Deserialize(world, buf, NullEntity, newActiveCamera);

		if (newRoot == NullEntity) {
			return false;
		}

		scene.SetSceneRootId(newRoot);
		scene.SetActiveCameraId(newActiveCamera);
		return true;
	}

}
