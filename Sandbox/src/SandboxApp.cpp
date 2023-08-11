#include <Iberus.h>

using namespace Math;

class Sandbox : public Iberus::Application {
public:
	Sandbox() {
	}

	void Boot() override {
		Application::Boot();

		auto& resourceManager = Iberus::Engine::Instance()->GetResourceManager();
		auto* provider = &Iberus::Engine::Instance()->GetEngineProvider();
		Iberus::Shader* shader = resourceManager.GetOrCreateResource<Iberus::Shader>("assets/shaders/baseShader", provider);
		Iberus::Mesh* mesh = resourceManager.GetOrCreateResource<Iberus::Mesh>("assets/meshes/coral1.obj", provider);
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
		material2->albedoColor = Vec4(1.0f, 0.3f, 0.3f, 1.0f);
		material2->SetTexture("albedoTexture", texture2);


		material3->SetShader(shader);
		material3->albedoColor = Vec4(0.0f, 0.3f, 0.45f, 0.5f);
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

		entity->SetPosition(Vec3(0, -10, 30));
		entity2->SetPosition(Vec3(8, 0, 30));
		entity3->SetPosition(Vec3(-8, 0, 30));

		entity->SetRotation(Vec3(0, 45, 0));
		entity->SetScale(Vec3(0.1, 0.1, 0.1));

		entity2->SetRotation(Vec3(0, 45, 45));
		entity2->SetScale(Vec3(0.1, 0.1, 0.1));

		entity3->SetRotation(Vec3(45, 45, 0));
		entity3->SetScale(Vec3(0.1, 0.1, 0.1));

		currentScene->AddEntity("Teste", entity);
		currentScene->AddEntity("Teste2", entity2);
		currentScene->AddEntity("Teste3", entity3);
	}

	void Update() override {
		Application::Update();
	}

	~Sandbox() {
	}

	Iberus::Scene* currentScene;
};
 
Iberus::Application* Iberus::CreateApplication() {
	return new Sandbox();
}