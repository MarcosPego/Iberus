#include "Enginepch.h"
#include "TerrainSystem.h"
#include "Engine.h"
#include "MeshFactory.h"
#include "Mesh.h"
#include "Noise.h"
#include "Components.h"

namespace Iberus {

	void TerrainSystem::Update(World& world) {
		auto* terrainStorage = world.GetStorage<TerrainComponent>();
		if (!terrainStorage) {
			return;
		}

		auto& resourceManager = Engine::Instance()->GetResourceManager();

		for (auto [entityId, terrain] : *terrainStorage) {
			if (!terrain.NeedsRegenerate || !world.IsAlive(entityId)) {
				continue;
			}

			terrain.NeedsRegenerate = false;

			int w = std::max(2, terrain.Width);
			int h = std::max(2, terrain.Height);

			std::vector<float> heightMap;
			Math::Noise::Instance()->GetHeightMap2D(heightMap, w, h, terrain.XOffset, terrain.ZOffset,
				terrain.Frequency, terrain.Seed, std::max(1, terrain.Octaves));

			Mesh* mesh = MeshFactory::CreatePlane(terrain.MeshId, resourceManager, w, h, heightMap,
				terrain.HeightScale, terrain.WorldSizeX, terrain.WorldSizeZ);

			if (mesh) {
				auto* meshRenderer = world.GetComponent<MeshRendererComponent>(entityId);
				if (meshRenderer) {
					meshRenderer->MeshId = terrain.MeshId;
					meshRenderer->MaterialId = terrain.MaterialId;
				} else {
					world.AddComponent<MeshRendererComponent>(entityId)->MeshId = terrain.MeshId;
					world.GetComponent<MeshRendererComponent>(entityId)->MaterialId = terrain.MaterialId;
				}
			}
		}
	}

}
