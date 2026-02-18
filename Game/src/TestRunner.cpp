#include <Iberus.h>

#include "GameApp.h"
#include "Scene/Serialization/SceneSerializer.h"
#include "Scene/Resources/MeshFactory.h"
#include "Project.h"
#include "ProjectSerializer.h"

#include <filesystem>
#include <cstring>
#include <iostream>

namespace {

	static size_t CountEntitiesWithTag(const Iberus::World& world) {
		auto* storage = world.GetStorage<Iberus::TagComponent>();
		return storage ? storage->Size() : 0;
	}

	void RunIntegrationTests() {
		int failures = 0;
		auto* engine = Iberus::Engine::Instance();
		if (!engine) {
			std::cerr << "FAIL: Engine::Instance() is null\n";
			std::exit(3);
		}

		auto& sceneMgr = engine->GetSceneManager();
		auto& resourceMgr = engine->GetResourceManager();
		auto* provider = &engine->GetEngineProvider();

		// --- Smoke: SceneManager ---
		auto* scene1 = sceneMgr.CreateScene("IntegrationTestScene", true);
		if (!scene1) {
			std::cerr << "FAIL: CreateScene returned null\n";
			++failures;
		}

		// --- Scene round-trip with hierarchy ---
		if (scene1) {
			// Build: root -> [Camera, Light, ChildA -> [ChildB]]
			Iberus::EntityId childA = scene1->CreateEntityECS("ChildA");
			scene1->AddChildECS(scene1->GetSceneRootId(), childA, "ChildA");
			if (auto* t = scene1->GetComponent<Iberus::TransformComponent>(childA)) {
				t->Position = Math::Vec3(1, 2, 3);
				t->Scale = Math::Vec3(2, 2, 2);
			}
			Iberus::EntityId childB = scene1->CreateEntityECS("ChildB");
			scene1->AddChildECS(childA, childB, "ChildB");
			if (auto* t = scene1->GetComponent<Iberus::TransformComponent>(childB)) {
				t->Position = Math::Vec3(10, 20, 30);
			}
			size_t entityCountBefore = CountEntitiesWithTag(scene1->GetWorld());

			Iberus::Buffer buf = Iberus::SceneSerializer::Serialize(*scene1);
			if (buf.Invalid()) {
				std::cerr << "FAIL: SceneSerializer::Serialize returned invalid buffer\n";
				++failures;
			} else {
				auto* scene2 = sceneMgr.CreateScene("SerializationTestScene", false);
				bool ok = Iberus::SceneSerializer::RestoreFromBuffer(*scene2, buf);
				if (!ok || scene2->GetSceneRootId() == Iberus::NullEntity) {
					std::cerr << "FAIL: RestoreFromBuffer failed or root is null\n";
					++failures;
				} else {
					size_t entityCountAfter = CountEntitiesWithTag(scene2->GetWorld());
					if (entityCountAfter != entityCountBefore) {
						std::cerr << "FAIL: Entity count mismatch after round-trip (before="
							<< entityCountBefore << ", after=" << entityCountAfter << ")\n";
						++failures;
					}
					// Verify root has children (Camera, Light, ChildA)
					auto* rootHier = scene2->GetComponent<Iberus::HierarchyComponent>(scene2->GetSceneRootId());
					if (!rootHier || rootHier->ChildrenIds.size() < 2) {
						std::cerr << "FAIL: Root hierarchy missing or has too few children\n";
						++failures;
					}
				}
			}
		}

		// --- API contract: LoadFromFile invalid path (no crash, returns false) ---
		{
			auto* scene = sceneMgr.CreateScene("InvalidPathTest", false);
			bool loaded = Iberus::SceneSerializer::LoadFromFile(*scene, "nonexistent/path/to/scene.scene");
			if (loaded) {
				std::cerr << "FAIL: LoadFromFile should return false for invalid path\n";
				++failures;
			}
		}

		// --- FileSystem ---
		{
			std::string assetsPath = Iberus::FileSystem::GetAssetsPath();
			if (assetsPath.empty()) {
				std::cerr << "FAIL: GetAssetsPath returned empty\n";
				++failures;
			}
			auto entries = Iberus::FileSystem::ListDirectory(assetsPath);
			if (entries.empty() && std::filesystem::exists(assetsPath)) {
				std::cerr << "WARN: ListDirectory returned empty for assets (may be OK if empty dir)\n";
			}
		}

		// --- ResourceManager: existing shader (loaded during Boot) ---
		{
			auto* shader = resourceMgr.GetResource<Iberus::Shader>("assets/shaders/baseGeometryShader");
			if (!shader) {
				std::cerr << "FAIL: GetResource for baseGeometryShader returned null (expected after Boot)\n";
				++failures;
			}
		}

		// --- ResourceManager: missing file (no crash, returns null) ---
		{
			auto* badMesh = resourceMgr.GetOrCreateResource<Iberus::Mesh>("nonexistent_mesh_12345", provider);
			if (badMesh) {
				std::cerr << "FAIL: GetOrCreateResource for nonexistent file should return null\n";
				++failures;
			}
		}

		// --- SaveToFile / LoadFromFile round-trip ---
		{
			auto* sceneWrite = sceneMgr.CreateScene("FileRoundTrip", false);
			Iberus::EntityId e = sceneWrite->CreateEntityECS("FileTestEntity");
			sceneWrite->AddChildECS(sceneWrite->GetSceneRootId(), e, "FileTestEntity");
			std::string testPath = "Scenes/IntegrationTest_FileRoundTrip.scene";
			std::string fullPath = Iberus::FileSystem::GetAssetsPath() + "/" + testPath;
			std::filesystem::create_directories(std::filesystem::path(fullPath).parent_path());
			bool saved = Iberus::SceneSerializer::SaveToFile(*sceneWrite, fullPath);
			if (!saved) {
				std::cerr << "FAIL: SaveToFile failed\n";
				++failures;
			} else {
				auto* sceneRead = sceneMgr.CreateScene("FileRoundTripRead", false);
				bool loaded = Iberus::SceneSerializer::LoadFromFile(*sceneRead, fullPath);
				if (!loaded) {
					std::cerr << "FAIL: LoadFromFile failed after SaveToFile\n";
					++failures;
				} else if (CountEntitiesWithTag(sceneRead->GetWorld()) < 2) {
					std::cerr << "FAIL: Loaded scene has too few entities\n";
					++failures;
				}
				std::filesystem::remove(fullPath);
			}
		}

		// --- Create project "test project", scene, cube with mesh+material ---
		{
			std::string exeDir = Iberus::FileSystem::GetExeDirectory();
			std::filesystem::path projectsDir = std::filesystem::path(exeDir) / "Projects";
			std::filesystem::path projectDir = projectsDir / "test project";
			std::filesystem::path projectFile = projectDir / "test project.project";

			// Create project folder and copy Demo assets
			if (std::filesystem::exists(projectDir)) {
				std::filesystem::remove_all(projectDir);
			}
			std::filesystem::create_directories(projectDir);
			std::filesystem::path demoAssets = projectsDir / "Demo" / "Assets";
			if (std::filesystem::exists(demoAssets)) {
				std::filesystem::copy(demoAssets, projectDir / "Assets", std::filesystem::copy_options::recursive);
			} else {
				std::filesystem::create_directories(projectDir / "Assets" / "Scenes");
			}
			auto project = std::make_unique<Iberus::Project>("test project");
			project->SetRootPath(".");
			project->SetCurrentScenePath("Scenes/TestScene.scene");
			if (!Iberus::ProjectSerializer::SaveToFile(*project, projectFile.string())) {
				std::cerr << "FAIL: Could not save test project\n";
				++failures;
			} else {
				// Switch working dir to test project
				std::filesystem::current_path(projectDir);
				engine->GetEngineProvider().SetWorkingDir(projectDir.string());

				auto* scene = sceneMgr.CreateScene("TestProjectScene", true);
				if (!scene) {
					std::cerr << "FAIL: CreateScene failed in test project\n";
					++failures;
				} else {
					// Create cube mesh
					const std::string cubeMeshId = "test_cube_mesh";
					auto* cubeMesh = Iberus::MeshFactory::CreateCube(cubeMeshId, resourceMgr);
					if (!cubeMesh) {
						std::cerr << "FAIL: CreateCube mesh failed\n";
						++failures;
					}
					// Create material with color
					const std::string matId = "TestMaterial_Red";
					auto* mat = resourceMgr.CreateResource<Iberus::Material>(matId);
					if (!mat) {
						std::cerr << "FAIL: Create material failed\n";
						++failures;
					} else {
						mat->albedoColor = Math::Vec4(0.9f, 0.2f, 0.2f, 1.0f);
						auto* shader = resourceMgr.GetResource<Iberus::Shader>("assets/shaders/baseGeometryShader");
						if (shader) {
							mat->SetShader(shader);
						}
					}
					// Spawn cube entity
					Iberus::EntityId cubeId = scene->CreateEntityECS("Cube");
					scene->AddChildECS(scene->GetSceneRootId(), cubeId, "Cube");
					auto* meshRenderer = scene->AddComponent<Iberus::MeshRendererComponent>(cubeId);
					if (meshRenderer) {
						meshRenderer->MeshId = cubeMeshId;
						meshRenderer->MaterialId = matId;
					}
					// Verify we have the entity with expected components
					if (!scene->GetComponent<Iberus::MeshRendererComponent>(cubeId) ||
						scene->GetComponent<Iberus::MeshRendererComponent>(cubeId)->MeshId != cubeMeshId) {
						std::cerr << "FAIL: Cube entity missing MeshRenderer or wrong MeshId\n";
						++failures;
					}
				}
				// Restore working dir
				std::filesystem::current_path(exeDir);
			}
		}

		if (failures > 0) {
			std::cerr << "Integration tests: " << failures << " failure(s)\n";
			std::exit(3);
		}
		std::cout << "Integration tests: passed (scene hierarchy, API contract, FileSystem, ResourceManager, file round-trip)\n";
	}

	class TestRunnerApp : public Iberus::Application {
	public:
		TestRunnerApp() : Application(GetTestWindowProps()) {
		}

		void Boot() override {
			// Use Projects/Demo as working dir so assets (shaders, etc.) resolve when running from Game-Build
			std::string exeDir = Iberus::FileSystem::GetExeDirectory();
			std::string projectDir = exeDir + "/Projects/Demo";
			if (std::filesystem::exists(projectDir)) {
				std::filesystem::current_path(projectDir);
			}
			Application::Boot();
			RunIntegrationTests();
			RequestExit();
		}

		void Update() override {
		}

	private:
		static Iberus::WindowProps GetTestWindowProps() {
			Iberus::WindowProps props;
			props.hidden = true;
			props.resolution = Math::Vec2(640, 480);
			return props;
		}
	};

}

Iberus::Application* Iberus::CreateApplication(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		if (std::strcmp(argv[i], "--run-tests") == 0) {
			return new TestRunnerApp();
		}
	}
	return Iberus::CreateGameApplication();
}
