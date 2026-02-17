#pragma once

#include "Core.h"
#include "Buffer.h"
#include "EntityId.h"
#include "JsonValue.h"

#include <string>

namespace Iberus {

	class World;
	class Scene;

	class IBERUS_API SceneSerializer {
	public:
		/// Serialize World to Buffer. If rootId != NullEntity, serialize that subtree; otherwise serialize from sceneRootId.
		static Buffer Serialize(const World& world, EntityId sceneRootId, EntityId activeCameraId, EntityId rootId = NullEntity);

		/// Deserialize from Buffer into World. Clears world first if clearWorld. Returns root EntityId and sets outActiveCameraId.
		static EntityId Deserialize(World& world, const Buffer& buffer, EntityId parentId, EntityId& outActiveCameraId);

		/// Convenience: save scene to file.
		static bool SaveToFile(const Scene& scene, const std::string& path);

		/// Convenience: load scene from file. Clears scene world and loads. Returns true on success.
		static bool LoadFromFile(Scene& scene, const std::string& path);
	};

}
