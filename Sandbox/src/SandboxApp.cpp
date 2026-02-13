#include <Iberus.h>

#include "GameLayer.h"
#include "EditorLayer.h"
#include "CustomSceneCreator.h"

class Sandbox : public Iberus::Application {
public:
	Sandbox() {
	}

	void Boot() override {
		Application::Boot();
		PushLayer(new Iberus::GameLayer());
		PushOverlay(new Iberus::EditorLayer());
		Iberus::CustomSceneCreator::Create();
	}

	void Update() override {
		Application::Update();
	}
};
 
Iberus::Application* Iberus::CreateApplication() {
	return new Sandbox();
}