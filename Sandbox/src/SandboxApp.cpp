#include <Iberus.h>

#include "CustomSceneCreator.h"

class Sandbox : public Iberus::Application {
public:
	Sandbox() {
	}

	void Boot() override {
		Application::Boot();
		Iberus::CustomSceneCreator::Create();
	}

	void Update() override {
		Application::Update();
	}
};
 
Iberus::Application* Iberus::CreateApplication() {
	return new Sandbox();
}