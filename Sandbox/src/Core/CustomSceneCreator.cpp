#include "CustomSceneCreator.h"
#include "SceneViewerCameraBehaviour.h"
#include "SandboxBehaviour2.h"

#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Components.h"

using namespace Math;

namespace Iberus {

	void CustomSceneCreator::Create() {
		auto& resourceManager = Engine::Instance()->GetResourceManager();
		auto* provider = &Engine::Instance()->GetEngineProvider();
		Shader* shader = resourceManager.GetOrCreateResource<Shader>("assets/shaders/baseLitShader", provider);
		resourceManager.GetOrCreateResource<Mesh>("assets/meshes/cube.obj", provider);
		Texture* texture1 = resourceManager.GetOrCreateResource<Texture>("assets/textures/texExample1.png", provider);
		Texture* texture2 = resourceManager.GetOrCreateResource<Texture>("assets/textures/texExample2.png", provider);
		Texture* texture3 = resourceManager.GetOrCreateResource<Texture>("assets/textures/texExample3.png", provider);

		auto* currentScene = Engine::Instance()->GetSceneManager().CreateScene("TestScene", true);

		// Camera: at (0,0,-5) looking toward +Z
		EntityId cameraId = currentScene->GetActiveCameraId();
		if (auto* transform = currentScene->GetComponent<TransformComponent>(cameraId)) {
			transform->Position = Vec3(0, 0, -5);
			transform->Rotation = Vec3(0, 180, 0);
		}
		currentScene->PushBehaviour(cameraId, new SceneViewerCameraBehaviour());

		// Materials
		Material* sdfmaterial1 = currentScene->GetOrCreateMaterial<Material>("SDFMaterial1");
		Material* sdfmaterial2 = currentScene->GetOrCreateMaterial<Material>("SDFMaterial2");
		Material* sdfmaterial3 = currentScene->GetOrCreateMaterial<Material>("SDFMaterial3");
		sdfmaterial1->albedoColor = Vec4(0, 0.8, 1, 1);
		sdfmaterial2->albedoColor = Vec4(0.34, 0.45, 1.0f, 1);
		sdfmaterial3->albedoColor = Vec4(0.24, 0, 0.67, 1);

		Material* cubeMat1 = currentScene->GetOrCreateMaterial<Material>("CubeMaterial1");
		Material* cubeMat2 = currentScene->GetOrCreateMaterial<Material>("CubeMaterial2");
		Material* cubeMat3 = currentScene->GetOrCreateMaterial<Material>("CubeMaterial3");
		cubeMat1->SetShader(shader);
		cubeMat1->SetTexture("albedoTexture", texture1);
		cubeMat2->SetShader(shader);
		cubeMat2->SetTexture("albedoTexture", texture2);
		cubeMat3->SetShader(shader);
		cubeMat3->SetTexture("albedoTexture", texture3);

		Material* planeMat = currentScene->GetOrCreateMaterial<Material>("PlaneMaterial");
		planeMat->SetShader(shader);
		planeMat->SetTexture("albedoTexture", texture2);
		planeMat->albedoColor = Vec4(0.5f, 0.5f, 0.5f, 1.0f);

		// --- SDF entity (kept as-is) ---
		{
			EntityId sdfEntityId = currentScene->CreateEntityECS("SDFteste1");

			currentScene->AddComponent<MeshRendererComponent>(sdfEntityId)->MaterialId = "SDFMaterial1";

			if (auto* transform = currentScene->GetComponent<TransformComponent>(sdfEntityId)) {
				transform->Position = Vec3(0, 5, 20);
			}

			currentScene->PushBehaviour(sdfEntityId, new SandboxBehaviour2());

			auto* sdfComponent = currentScene->AddComponent<SDFComponent>(sdfEntityId);
			if (sdfComponent) {
				auto pushPart = [&](Vec3 pos, int type, float radius, const std::string& matId) {
					SDFPartData part;
					part.Transform.Position = pos;
					part.Type = type;
					part.Radius = radius;
					part.MaterialId = matId;
					sdfComponent->Parts.push_back(part);
				};
				pushPart(Vec3(-20, 0, 20), 1, 1.0f, "SDFMaterial1");
				pushPart(Vec3(-20, 0, 20), 1, 1.0f, "SDFMaterial1");
				pushPart(Vec3(-20, 0, 20), 1, 1.0f, "SDFMaterial2");
				pushPart(Vec3(-20, 0, 20), 1, 1.0f, "SDFMaterial2");
				pushPart(Vec3(-20, 0, 20), 1, 1.0f, "SDFMaterial2");
			}

			currentScene->AddChildECS(currentScene->GetSceneRootId(), sdfEntityId, "SDFteste1");
		}

		// --- Cubes around the SDF ---
		auto createCube = [&](const std::string& tagId, Vec3 pos, const std::string& matId) {
			EntityId id = currentScene->CreateEntityECS(tagId);
			auto* meshRenderer = currentScene->AddComponent<MeshRendererComponent>(id);
			meshRenderer->MeshId = "assets/meshes/cube.obj";
			meshRenderer->MaterialId = matId;
			if (auto* t = currentScene->GetComponent<TransformComponent>(id)) {
				t->Position = pos;
			}
			currentScene->AddChildECS(currentScene->GetSceneRootId(), id, tagId);
		};
		createCube("Cube1", Vec3(-8, 3, 18), "CubeMaterial1");
		createCube("Cube2", Vec3(8, 4, 22), "CubeMaterial2");
		createCube("Cube3", Vec3(-5, 2, 25), "CubeMaterial1");
		createCube("Cube4", Vec3(6, 5, 16), "CubeMaterial3");
		createCube("Cube5", Vec3(-10, 1, 20), "CubeMaterial2");

		// --- Terrain (displaced height map, editable via TerrainComponent in Inspector) ---
		{
			EntityId terrainId = currentScene->CreateEntityECS("Terrain");
			currentScene->AddComponent<TerrainComponent>(terrainId);
			if (auto* t = currentScene->GetComponent<TransformComponent>(terrainId)) {
				t->Position = Vec3(0, 0, 50);
				t->Rotation = Vec3(0, 0, 0);
				t->Scale = Vec3(1, 1, 1);
			}
			currentScene->AddChildECS(currentScene->GetSceneRootId(), terrainId, "Terrain");
		}
	}

}
