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

		Iberus::Mesh* mesh = resourceManager.GetOrCreateResource<Iberus::Mesh>("assets/meshes/cube.obj", provider);


		auto* currentScene = Iberus::Engine::Instance()->GetSceneManager().CreateScene("TestScene", true);
		Iberus::Entity* entity = currentScene->CreateEntity<Iberus::Entity>("Teste");
		Iberus::Entity* entity2 = currentScene->CreateEntity<Iberus::Entity>("Teste2");
		Iberus::Entity* entity3 = currentScene->CreateEntity<Iberus::Entity>("Teste3");
		entity->mesh = mesh;
		entity->shader = shader;

		entity2->mesh = mesh;
		entity2->shader = shader;

		entity3->mesh = mesh;
		entity3->shader = shader;

		entity->SetPosition(Vec3(0, 0, 30));
		entity2->SetPosition(Vec3(8, 0, 30));
		entity3->SetPosition(Vec3(-8, 0, 30));

		entity->SetRotation(Vec3(0, 45, 0));
		entity->SetScale(Vec3(2, 5, 2));

		entity2->SetRotation(Vec3(0, 45, 45));
		entity2->SetScale(Vec3(5, 1, 2));

		entity3->SetRotation(Vec3(45, 45, 0));
		entity3->SetScale(Vec3(5, 5, 2));

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