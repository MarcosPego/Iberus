#include <Iberus.h>

#include "GameApp.h"
#include "Scene/Serialization/SceneSerializer.h"

#include "GameLayer.h"
#include "EditorLayer.h"
#include "SceneViewerCameraBehaviour.h"
#include "Maths/MathUtils.h"

#include <filesystem>

namespace {

	void OpenProject(Iberus::Project* project);

	void SetupSceneWithBehaviours(Iberus::Scene* scene) {
		if (!scene) {
			return;
		}
		Iberus::EntityId cameraId = scene->GetActiveCameraId();
		if (cameraId != Iberus::NullEntity) {
			if (auto* transform = scene->GetComponent<Iberus::TransformComponent>(cameraId)) {
				transform->Position = Math::Vec3(0, 0, -5);
				transform->Rotation = Math::Vec3(0, 180, 0);
			}
			scene->PushBehaviour(cameraId, new Iberus::SceneViewerCameraBehaviour());
		}
	}

	void OpenProject(Iberus::Project* project) {
		if (!project) {
			return;
		}
		std::string rootPath = project->GetRootPath();
		Iberus::Engine::Instance()->SetScriptBaseDir(rootPath);
		std::filesystem::current_path(rootPath);
		Iberus::Engine::Instance()->GetEngineProvider().SetWorkingDir(rootPath);

		Iberus::Engine::Instance()->BuildProjectScripts(rootPath);

		std::string scenePath = project->GetCurrentScenePath();
		if (scenePath.empty()) {
			scenePath = "Scenes/TestScene.scene";
		}
		std::string fullScenePath;
		if (scenePath[0] == '/' || scenePath.find(':') != std::string::npos) {
			fullScenePath = scenePath;
		} else {
			fullScenePath = project->GetAssetsPath() + "/" + scenePath;
		}

		auto* scene = Iberus::Engine::Instance()->GetSceneManager().CreateScene("TestScene", true);
		if (std::filesystem::exists(fullScenePath)) {
			Iberus::SceneSerializer::LoadFromFile(*scene, fullScenePath);
		}
		SetupSceneWithBehaviours(scene);
		if (!std::filesystem::exists(fullScenePath)) {
			std::filesystem::path scenesDir = std::filesystem::path(project->GetAssetsPath()) / "Scenes";
			std::filesystem::create_directories(scenesDir);
			Iberus::SceneSerializer::SaveToFile(*scene, fullScenePath);
		}
		Iberus::Application::Get()->SetCurrentScenePath(fullScenePath);
	}

}

class Game : public Iberus::Application {
public:
	Game() {
	}

	void Boot() override {
		Application::Boot();
		PushLayer(new Iberus::GameLayer());
		PushOverlay(new Iberus::EditorLayer());
	}

	void OnProjectChosen(Iberus::Project* openedProject) override {
		if (openedProject) {
			OpenProject(openedProject);
		}
	}

	void Update() override {
		Application::Update();
	}

	void OnCloseProject() override {
		Iberus::Engine::Instance()->GetSceneManager().Clear();
		Iberus::Engine::Instance()->SetScriptBaseDir("");
		SetProject(nullptr);
		SetCurrentScenePath("");
		SetSceneDirty(false);
		RequestProjectScreen();
	}
};

Iberus::Application* Iberus::CreateGameApplication() {
	return new Game();
}
