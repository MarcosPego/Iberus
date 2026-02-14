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
		Mesh* mesh = resourceManager.GetOrCreateResource<Mesh>("assets/meshes/cube.obj", provider);
		Texture* texture1 = resourceManager.GetOrCreateResource<Texture>("assets/textures/texExample1.png", provider);
		Texture* texture2 = resourceManager.GetOrCreateResource<Texture>("assets/textures/texExample2.png", provider);
		Texture* texture3 = resourceManager.GetOrCreateResource<Texture>("assets/textures/texExample3.png", provider);

		auto* currentScene = Engine::Instance()->GetSceneManager().CreateScene("TestScene", true);

		// Match original working view: camera at (0,0,-5) looking toward +Z (worm at 0,5,20)
		EntityId cameraId = currentScene->GetActiveCameraId();
		if (auto* transform = currentScene->GetComponent<TransformComponent>(cameraId)) {
			transform->Position = Vec3(0, 0, -5);
			transform->Rotation = Vec3(0, 180, 0);
		}
		currentScene->PushBehaviour(cameraId, new SceneViewerCameraBehaviour());

		{
			EntityId sdfEntityId = currentScene->CreateEntityECS("SDFteste1");

			Material* sdfmaterial1 = currentScene->GetOrCreateMaterial<Material>("SDFMaterial1");
			Material* sdfmaterial2 = currentScene->GetOrCreateMaterial<Material>("SDFMaterial2");
			Material* sdfmaterial3 = currentScene->GetOrCreateMaterial<Material>("SDFMaterial3");

			sdfmaterial1->albedoColor = Vec4(0, 0.8, 1, 1);
			sdfmaterial2->albedoColor = Vec4(0.34, 0.45, 1.0f, 1);
			sdfmaterial3->albedoColor = Vec4(0.24, 0, 0.67, 1);

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
	}

}
