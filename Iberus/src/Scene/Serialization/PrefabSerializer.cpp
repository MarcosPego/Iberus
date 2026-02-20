#include "Enginepch.h"
#include "PrefabSerializer.h"
#include "PrefabAsset.h"
#include "SceneSerializer.h"
#include "World.h"
#include "FileSystem.h"

#include <fstream>

namespace Iberus {

	Buffer PrefabSerializer::Serialize(const World& world, EntityId rootId) {
		return SceneSerializer::Serialize(world, rootId, NullEntity, rootId);
	}

	PrefabAsset PrefabSerializer::Deserialize(Buffer buffer) {
		return PrefabAsset(std::move(buffer));
	}

	bool PrefabSerializer::SaveToFile(const PrefabAsset& prefab, const std::string& path) {
		const Buffer& buf = prefab.GetBuffer();
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

	bool PrefabSerializer::SaveToFile(const World& world, EntityId rootId, const std::string& path) {
		Buffer buf = Serialize(world, rootId);
		if (buf.Invalid()) {
			return false;
		}
		return SaveToFile(PrefabAsset(std::move(buf)), path);
	}

	PrefabAsset PrefabSerializer::LoadFromFile(const std::string& path) {
		Buffer buf = FileSystem::GetRawFileBuffer(path);
		return Deserialize(std::move(buf));
	}

	EntityId PrefabSerializer::Instantiate(const PrefabAsset& prefab, World& world, EntityId parentId) {
		if (!prefab.IsValid()) {
			return NullEntity;
		}
		EntityId outActiveCamera = NullEntity;
		return SceneSerializer::Deserialize(world, prefab.GetBuffer(), parentId, outActiveCamera);
	}

}
