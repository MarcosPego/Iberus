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
		/// When sceneForBehaviours is set, includes C++ behaviour types in the output.
		static Buffer Serialize(const World& world, EntityId sceneRootId, EntityId activeCameraId, EntityId rootId = NullEntity, const Scene* sceneForBehaviours = nullptr);

		/// Serialize full scene including behaviours.
		static Buffer Serialize(const Scene& scene, EntityId rootId = NullEntity);

		/// Deserialize from Buffer into World. Returns root EntityId and sets outActiveCameraId.
		/// When sceneForBehaviours is set, instantiates C++ behaviours via BehaviourRegistry.
		static EntityId Deserialize(World& world, const Buffer& buffer, EntityId parentId, EntityId& outActiveCameraId, Scene* sceneForBehaviours = nullptr);

		/// Convenience: save scene to file.
		static bool SaveToFile(const Scene& scene, const std::string& path);

		/// Convenience: load scene from file. Clears scene world and loads. Returns true on success.
		static bool LoadFromFile(Scene& scene, const std::string& path);

		/// Restore scene from buffer (destroy current, deserialize, preload resources). Used for Play/Stop.
		static bool RestoreFromBuffer(Scene& scene, const Buffer& buffer);
	};

}
