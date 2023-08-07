#include <Iberus.h>

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
		entity->mesh = mesh;
		entity->shader = shader;

		currentScene->AddEntity("Teste", entity);
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