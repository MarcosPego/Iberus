#include "CustomSceneCreator.h"
#include "SceneViewerCameraBehaviour.h"
#include "SandboxBehaviour2.h"

#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshFactory.h"
#include "Material.h"
#include "SDFEntity.h"
#include "Noise.h"

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
		currentScene->GetActiveCamera()->SetPosition(Vec3(0, 0, -5));
		currentScene->GetActiveCamera()->SetRotation(Vec3(0, 180, 0));
		currentScene->GetActiveCamera()->PushBehaviour(new SceneViewerCameraBehaviour());

		if (false) {
			Material* material1 = currentScene->GetOrCreateMaterial<Material>("Material1");
			Material* material2 = currentScene->GetOrCreateMaterial<Material>("Material2");
			Material* material3 = currentScene->GetOrCreateMaterial<Material>("Material3");

			material1->SetShader(shader);
			material1->SetTexture("albedoTexture", texture1);

			material2->SetShader(shader);
			material2->SetTexture("albedoTexture", texture2);

			material3->SetShader(shader);
			material3->SetTexture("albedoTexture", texture3);

			Entity* entity = currentScene->CreateEntity<Entity>("Teste");
			Entity* entity2 = currentScene->CreateEntity<Entity>("Teste2");
			Entity* entity3 = currentScene->CreateEntity<Entity>("Teste3");
			entity->SetMesh(mesh);
			entity->SetMaterial(material3);

			entity2->SetMesh(mesh);
			entity2->SetMaterial(material2);

			entity->SetPosition(Vec3(10, -10, 15));
			entity2->SetPosition(Vec3(15, -15, 15));
			entity3->SetPosition(Vec3(20, -10, 15));

			entity->SetRotation(Vec3(65.0f, 0, 0));
			entity->SetScale(Vec3(1.0f, 1.0f, 1));

			entity2->SetRotation(Vec3(45.0f, 0, 0));
			entity2->SetScale(Vec3(1, 1, 1));

			entity3->SetRotation(Vec3(15.0f, 0, 0));
			entity3->SetScale(Vec3(1.0f, 0.0f, 1.0f));

			auto sample = Math::Noise::Instance()->GetNoise({ 0,0,0 }, { 32 , 32 , 1 }, 1337, 0.05f, 4);
			Texture* noiseTexture = resourceManager.CreateResource<Texture>("noiseTest", std::move(Math::Noise::Instance()->SampleToTextureBuffer(sample)), 32, 32, 4);
			material3->SetTexture("albedoTexture", texture3);

			std::vector<float> vec(sample.noiseFloatBuffer.GetSize());
			std::memcpy(vec.data(), sample.noiseFloatBuffer.GetData(), sample.noiseFloatBuffer.GetSize());

			Mesh* planeMesh = MeshFactory::CreatePlane("planeMesh", resourceManager, 32, 32, vec);
			entity3->SetMesh(planeMesh);
			entity3->SetMaterial(material3);

			entity2->SetMesh(planeMesh);
			entity2->SetMaterial(material3);

			entity->SetMesh(planeMesh);
			entity->SetMaterial(material3);

			currentScene->AddEntity("Teste", entity);
			currentScene->AddEntity("Teste2", entity2);
			currentScene->AddEntity("Teste3", entity3);

		} else if (true) {
			SDFEntity* sdfentity1 = currentScene->CreateEntity<SDFEntity>("SDFteste1");

			Material* sdfmaterial1 = currentScene->GetOrCreateMaterial<Material>("SDFMaterial1");
			Material* sdfmaterial2 = currentScene->GetOrCreateMaterial<Material>("SDFMaterial2");
			Material* sdfmaterial3 = currentScene->GetOrCreateMaterial<Material>("SDFMaterial3");

			sdfmaterial1->albedoColor = Vec4(0, 0.8, 1, 1);
			sdfmaterial2->albedoColor = Vec4(0.34, 0.45, 1.0f, 1);
			sdfmaterial3->albedoColor = Vec4(0.24, 0, 0.67, 1);

			sdfentity1->SetMaterial(sdfmaterial1);
			sdfentity1->SetPosition(Vec3(0, 5, 20));
			sdfentity1->PushBehaviour(new SandboxBehaviour2());

			sdfentity1->PushPart(Vec3(-20, 0, 20), 1, 1.0f, sdfmaterial1);
			sdfentity1->PushPart(Vec3(-20, 0, 20), 1, 1.0f, sdfmaterial1);
			sdfentity1->PushPart(Vec3(-20, 0, 20), 1, 1.0f, sdfmaterial2);
			sdfentity1->PushPart(Vec3(-20, 0, 20), 1, 1.0f, sdfmaterial2);
			sdfentity1->PushPart(Vec3(-20, 0, 20), 1, 1.0f, sdfmaterial2);

			currentScene->AddEntity("SDFteste1", sdfentity1);
		}
	}

}
