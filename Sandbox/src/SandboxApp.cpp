#include <Iberus.h>

#include "SandboxBehaviour.h"

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
		material3->SetTexture("albedoTexture", texture3);

		Iberus::Entity* entity = currentScene->CreateEntity<Iberus::Entity>("Teste");
		Iberus::Entity* entity2 = currentScene->CreateEntity<Iberus::Entity>("Teste2");
		Iberus::Entity* entity3 = currentScene->CreateEntity<Iberus::Entity>("Teste3");
		entity->SetMesh(mesh);
		entity->SetMaterial(material1);

		entity2->SetMesh(mesh);
		entity2->SetMaterial(material2);

		entity3->SetMesh(mesh);
		entity3->SetMaterial(material3);

		entity->SetPosition(Vec3(4, -10, 30));
		entity2->SetPosition(Vec3(8, 0, 30));
		entity3->SetPosition(Vec3(-8, 0, 30));

		entity->SetRotation(Vec3(0, 45, 0));
		entity->SetScale(Vec3(1, 1, 1));

		entity2->SetRotation(Vec3(0, 45, 45));
		entity2->SetScale(Vec3(1, 1, 1));

		entity3->SetRotation(Vec3(45, 45, 0));
		entity3->SetScale(Vec3(1, 1, 1));

		currentScene->AddEntity("Teste", entity);
		currentScene->AddEntity("Teste2", entity2);
		currentScene->AddEntity("Teste3", entity3);

		/// Add SDF entities
		auto* sdfentity1 = currentScene->CreateEntity<Iberus::SDFEntity>("SDFteste1");
		auto* sdfentity2 = currentScene->CreateEntity<Iberus::SDFEntity>("SDFteste2");
		auto* sdfentity3 = currentScene->CreateEntity<Iberus::SDFEntity>("SDFteste3");

		auto* sdfmaterial1 = currentScene->GetOrCreateMaterial<Iberus::Material>("SDFMaterial1");
		auto* sdfmaterial2 = currentScene->GetOrCreateMaterial<Iberus::Material>("SDFMaterial2");
		auto* sdfmaterial3 = currentScene->GetOrCreateMaterial<Iberus::Material>("SDFMaterial3");

		sdfmaterial1->albedoColor = Vec4(0, 0.8, 1, 1);
		sdfmaterial2->albedoColor = Vec4(0.34, 0.45, 0, 1);
		sdfmaterial3->albedoColor = Vec4(0.24, 0, 0.67, 1);

		sdfentity1->SetMaterial(sdfmaterial1);
		sdfentity2->SetMaterial(sdfmaterial2);
		sdfentity3->SetMaterial(sdfmaterial3);
		
		sdfentity1->SetPosition(Vec3(0, 0, 20));
		sdfentity2->SetPosition(Vec3(-10, -10, 20));
		sdfentity3->SetPosition(Vec3(10, 10, 20));

		sdfentity1->PushBehaviour(new SandboxBehaviour());

		currentScene->AddEntity("SDFteste1", sdfentity1);
		currentScene->AddEntity("SDFteste2", sdfentity2);
		currentScene->AddEntity("SDFteste3", sdfentity3);
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