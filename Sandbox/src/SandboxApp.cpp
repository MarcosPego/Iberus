#include <Iberus.h>

#include "SandboxBehaviour.h"
#include "SandboxBehaviour2.h"

using namespace Math;

class Sandbox : public Iberus::Application {
public:
	Sandbox() {
	}

	void Boot() override {
		Application::Boot();
		auto& resourceManager = Iberus::Engine::Instance()->GetResourceManager();
		auto* provider = &Iberus::Engine::Instance()->GetEngineProvider();
		Iberus::Shader* shader = resourceManager.GetOrCreateResource<Iberus::Shader>("assets/shaders/baseLitShader", provider);
		Iberus::Mesh* mesh = resourceManager.GetOrCreateResource<Iberus::Mesh>("assets/meshes/cube.obj", provider);
		Iberus::Texture* texture1 = resourceManager.GetOrCreateResource<Iberus::Texture>("assets/textures/texExample1.png", provider);
		Iberus::Texture* texture2 = resourceManager.GetOrCreateResource<Iberus::Texture>("assets/textures/texExample2.png", provider);
		Iberus::Texture* texture3 = resourceManager.GetOrCreateResource<Iberus::Texture>("assets/textures/texExample3.png", provider);

		auto* currentScene = Iberus::Engine::Instance()->GetSceneManager().CreateScene("TestScene", true);

		currentScene->GetActiveCamera()->SetPosition(Vec3(0, 0, 10));


		if (false) {
			Iberus::Material* material1 = currentScene->GetOrCreateMaterial<Iberus::Material>("Material1");
			Iberus::Material* material2 = currentScene->GetOrCreateMaterial<Iberus::Material>("Material2");
			Iberus::Material* material3 = currentScene->GetOrCreateMaterial<Iberus::Material>("Material3");

			material1->SetShader(shader);
			material1->SetTexture("albedoTexture", texture1);

			material2->SetShader(shader);
			//material2->albedoColor = Vec4(1.0f, 0.3f, 0.3f, 1.0f);
			material2->SetTexture("albedoTexture", texture2);

			material3->SetShader(shader);
			//material3->albedoColor = Vec4(0.0f, 0.3f, 0.45f, 0.5f);
			//material3->SetTexture("albedoTexture", texture3);

			Iberus::Entity* entity = currentScene->CreateEntity<Iberus::Entity>("Teste");
			Iberus::Entity* entity2 = currentScene->CreateEntity<Iberus::Entity>("Teste2");
			Iberus::Entity* entity3 = currentScene->CreateEntity<Iberus::Entity>("Teste3");
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
			Iberus::Texture* noiseTexture = resourceManager.CreateResource<Iberus::Texture>("noiseTest", std::move(Math::Noise::Instance()->SampleToTextureBuffer(sample)), 32, 32, 4);
			material3->SetTexture("albedoTexture", texture3);

			std::vector<float> vec(sample.noiseFloatBuffer.GetSize());
			std::memcpy(vec.data(), sample.noiseFloatBuffer.GetData(), sample.noiseFloatBuffer.GetSize());

			auto* planeMesh = Iberus::MeshFactory::CreatePlane("planeMesh", resourceManager, 32, 32, vec);
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
			auto* sdfentity1 = currentScene->CreateEntity<Iberus::SDFEntity>("SDFteste1");
			//auto* sdfentity2 = currentScene->CreateEntity<Iberus::SDFEntity>("SDFteste2");
			//auto* sdfentity3 = currentScene->CreateEntity<Iberus::SDFEntity>("SDFteste3");

			auto* sdfmaterial1 = currentScene->GetOrCreateMaterial<Iberus::Material>("SDFMaterial1");
			auto* sdfmaterial2 = currentScene->GetOrCreateMaterial<Iberus::Material>("SDFMaterial2");
			auto* sdfmaterial3 = currentScene->GetOrCreateMaterial<Iberus::Material>("SDFMaterial3");

			sdfmaterial1->albedoColor = Vec4(0, 0.8, 1, 1);
			sdfmaterial2->albedoColor = Vec4(0.34, 0.45, 1.0f, 1);
			sdfmaterial3->albedoColor = Vec4(0.24, 0, 0.67, 1);

			sdfentity1->SetMaterial(sdfmaterial1);
			//sdfentity2->SetMaterial(sdfmaterial2);
			//sdfentity3->SetMaterial(sdfmaterial3);

			sdfentity1->SetPosition(Vec3(0, 5, 20));
			//sdfentity2->SetPosition(Vec3(-10, -10, 20));
			//sdfentity3->SetPosition(Vec3(10, 10, 20));

			sdfentity1->PushBehaviour(new SandboxBehaviour2());
			//sdfentity2->PushBehaviour(new SandboxBehaviour2());
			//sdfentity3->PushBehaviour(new SandboxBehaviour2());

			sdfentity1->PushPart(Vec3(-20, 0, 20), 1, 1.0f, sdfmaterial1);
			sdfentity1->PushPart(Vec3(-20, 0, 20), 1, 1.0f, sdfmaterial1);
			sdfentity1->PushPart(Vec3(-20, 0, 20), 1, 1.0f, sdfmaterial2);
			sdfentity1->PushPart(Vec3(-20, 0, 20), 1, 1.0f, sdfmaterial2);
			sdfentity1->PushPart(Vec3(-20, 0, 20), 1, 1.0f, sdfmaterial2);

			currentScene->AddEntity("SDFteste1", sdfentity1);
			//currentScene->AddEntity("SDFteste2", sdfentity2);
			//currentScene->AddEntity("SDFteste3", sdfentity3);
			
		}
	}

	void Update() override {
		Application::Update();
	}

	~Sandbox() {
		currentScene = nullptr;
	}

	Iberus::Scene* currentScene;
};
 
Iberus::Application* Iberus::CreateApplication() {
	return new Sandbox();
}