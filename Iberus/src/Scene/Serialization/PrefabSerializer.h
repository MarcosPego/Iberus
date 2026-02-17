#pragma once

#include "Core.h"
#include "Buffer.h"
#include "EntityId.h"

#include <string>

namespace Iberus {

	class World;
	class PrefabAsset;
	class Scene;

	class IBERUS_API PrefabSerializer {
	public:
		/// Serialize entity subtree to Buffer (same format as scene fragment).
		static Buffer Serialize(const World& world, EntityId rootId);

		/// Deserialize Buffer into PrefabAsset. Takes buffer by value (moved).
		static PrefabAsset Deserialize(Buffer buffer);

		static bool SaveToFile(const PrefabAsset& prefab, const std::string& path);
		static bool SaveToFile(const World& world, EntityId rootId, const std::string& path);

		static PrefabAsset LoadFromFile(const std::string& path);

		/// Instantiate prefab into world under parent. Returns root EntityId.
		static EntityId Instantiate(const PrefabAsset& prefab, World& world, EntityId parentId);
	};

}
