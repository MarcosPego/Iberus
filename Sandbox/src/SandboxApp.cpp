#include <Iberus.h>

class Sandbox : public Iberus::Application {
public:
	Sandbox() {
	}

	void Boot() override {
		Application::Boot();

		auto* currentScene = Iberus::Engine::Instance()->GetSceneManager().CreateScene("TestScene", true);
		Iberus::Entity* entity = currentScene->CreateEntity<Iberus::Entity>("Teste");
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