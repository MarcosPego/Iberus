#include <Iberus.h>

#include "GameLayer.h"
#include "EditorLayer.h"
#include "CustomSceneCreator.h"
#include "SceneViewerCameraBehaviour.h"
#include "SandboxBehaviour2.h"

#include <filesystem>
#include <functional>

namespace {

	void OpenProject(Iberus::Project* project);

	void SetupSceneWithBehaviours(Iberus::Scene* scene) {
		if (!scene) {
			return;
		}
		if (auto* cam = scene->GetComponent<Iberus::TagComponent>(scene->GetActiveCameraId())) {
			if (cam->Id == "Camera") {
				scene->PushBehaviour(scene->GetActiveCameraId(), new Iberus::SceneViewerCameraBehaviour());
			}
		}
		Iberus::EntityId sdfEntity = Iberus::NullEntity;
		std::function<void(Iberus::EntityId)> findSdf = [&](Iberus::EntityId eid) {
			if (auto* tag = scene->GetComponent<Iberus::TagComponent>(eid)) {
				if (tag->Id == "SDFteste1" && scene->HasComponent<Iberus::SDFComponent>(eid)) {
					sdfEntity = eid;
					return;
				}
			}
			if (auto* hierarchy = scene->GetComponent<Iberus::HierarchyComponent>(eid)) {
				for (Iberus::EntityId cid : hierarchy->ChildrenIds) {
					findSdf(cid);
					if (sdfEntity != Iberus::NullEntity) {
						return;
					}
				}
			}
		};
		findSdf(scene->GetSceneRootId());
		if (sdfEntity != Iberus::NullEntity) {
			scene->PushBehaviour(sdfEntity, new SandboxBehaviour2());
		}
	}

	void OpenProject(Iberus::Project* project) {
		if (!project) {
			return;
		}
		std::string rootPath = project->GetRootPath();
		std::filesystem::current_path(rootPath);
		Iberus::Engine::Instance()->GetEngineProvider().SetWorkingDir(rootPath);

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
		Iberus::CustomSceneCreator::SetupResourcesAndMaterials(scene);

		if (std::filesystem::exists(fullScenePath)) {
			if (Iberus::SceneSerializer::LoadFromFile(*scene, fullScenePath)) {
				SetupSceneWithBehaviours(scene);
			} else {
				Iberus::CustomSceneCreator::Create();
			}
		} else {
			Iberus::CustomSceneCreator::Create();
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
		SetProject(nullptr);
		SetCurrentScenePath("");
		SetSceneDirty(false);
		RequestProjectScreen();
	}
};

Iberus::Application* Iberus::CreateApplication() {
	return new Game();
}
