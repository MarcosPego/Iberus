#include "Enginepch.h"
#include "WelcomePanel.h"
#include "Application.h"
#include "Project.h"
#include "ProjectSerializer.h"
#include "Engine.h"
#include "SceneManager.h"
#include "SceneSerializer.h"
#include "FileSystem.h"

#include "imgui.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <cctype>

namespace Iberus {

	WelcomePanel::WelcomePanel(Application& app)
		: application(app) {
		openProjectPath[0] = '\0';
		RefreshDiscoveredProjects();
	}

	bool WelcomePanel::TryCreateProject(const std::string& name) {
		if (name.empty()) {
			return false;
		}
		for (char c : name) {
			if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '-' && c != ' ') {
				return false;
			}
		}
		std::string exeDir = FileSystem::GetExeDirectory();
		std::filesystem::path projectsDir = std::filesystem::path(exeDir) / "Projects";
		std::filesystem::path newProjectDir = projectsDir / name;
		std::filesystem::path projectFile = newProjectDir / (name + ".project");
		if (std::filesystem::exists(newProjectDir)) {
			return false;
		}
		std::filesystem::create_directories(newProjectDir);
		std::filesystem::path demoAssets = projectsDir / "Demo" / "Assets";
		if (std::filesystem::exists(demoAssets)) {
			std::filesystem::path newAssets = newProjectDir / "Assets";
			std::filesystem::copy(demoAssets, newAssets, std::filesystem::copy_options::recursive);
		} else {
			std::filesystem::create_directories(newProjectDir / "Assets" / "Scenes");
		}
		std::filesystem::path demoScripts = projectsDir / "Demo" / "Assets" / "Scripts";
		if (std::filesystem::exists(demoScripts) && std::filesystem::is_directory(demoScripts)) {
			std::filesystem::path newScripts = newProjectDir / "Assets" / "Scripts";
			std::filesystem::copy(demoScripts, newScripts, std::filesystem::copy_options::recursive);
			std::filesystem::path oldCsproj = newScripts / "Demo.Scripts.csproj";
			std::filesystem::path newCsproj = newScripts / (name + ".Scripts.csproj");
			if (std::filesystem::exists(oldCsproj)) {
				auto replaceInFile = [&](const std::filesystem::path& p) {
					std::ifstream in(p);
					if (!in) { return; }
					std::ostringstream contents;
					contents << in.rdbuf();
					in.close();
					std::string str = contents.str();
					for (size_t pos = 0; (pos = str.find("Demo", pos)) != std::string::npos; pos += name.size()) {
						str.replace(pos, 4, name);
					}
					std::ofstream out(p);
					if (out) { out << str; }
				};
				replaceInFile(oldCsproj);
				std::filesystem::rename(oldCsproj, newCsproj);
				for (const auto& e : std::filesystem::directory_iterator(newScripts)) {
					if (e.path().extension() == ".cs") {
						replaceInFile(e.path());
					}
				}
			}
		}
		auto project = std::make_unique<Project>(name);
		project->SetRootPath(".");
		project->SetCurrentScenePath("Scenes/TestScene.scene");
		if (!ProjectSerializer::SaveToFile(*project, projectFile.string())) {
			return false;
		}
		RefreshDiscoveredProjects();
		return true;
	}

	void WelcomePanel::RefreshDiscoveredProjects() {
		discoveredProjectPaths.clear();
		selectedProjectIndex = -1;

		std::string exeDir = FileSystem::GetExeDirectory();
		std::filesystem::path projectsDir = std::filesystem::path(exeDir) / "Projects";
		if (!std::filesystem::exists(projectsDir) || !std::filesystem::is_directory(projectsDir)) {
			return;
		}

		for (const auto& entry : std::filesystem::directory_iterator(projectsDir)) {
			if (!entry.is_directory()) {
				continue;
			}
			for (const auto& file : std::filesystem::directory_iterator(entry.path())) {
				if (file.is_regular_file() && file.path().extension() == ".project") {
					discoveredProjectPaths.push_back(file.path().string());
					break;
				}
			}
		}
	}

	void WelcomePanel::OnDraw(IGUIContext& gui) {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 workPos = viewport->WorkPos;
		ImVec2 workSize = viewport->WorkSize;

		float panelWidth = 400.0f;
		float panelHeight = 380.0f;
		ImVec2 panelPos(
			workPos.x + (workSize.x - panelWidth) * 0.5f,
			workPos.y + (workSize.y - panelHeight) * 0.5f);

		ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_Always);
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

		if (ImGui::Begin("Welcome to Iberus", nullptr, flags)) {
			ImGui::Spacing();
			ImGui::TextWrapped("Open or create a project:");
			ImGui::Spacing();

			ImGui::Text("Create project:");
			ImGui::SetNextItemWidth(-1);
			ImGui::InputTextWithHint("##CreateName", "Project name", createProjectName, kNameBufferSize);
			if (ImGui::Button("Create", ImVec2(-1, 0))) {
				std::string name(createProjectName);
				if (TryCreateProject(name)) {
					application.DismissProjectScreen();
					auto project = ProjectSerializer::LoadFromFile(
						(std::filesystem::path(FileSystem::GetExeDirectory()) / "Projects" / name / (name + ".project")).string());
					if (project) {
						application.SetProject(std::move(project));
						application.OnProjectChosen(application.GetProject());
					}
				}
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Open project:");

			if (!discoveredProjectPaths.empty()) {
				std::string previewLabel = "(Select a project)";
				if (selectedProjectIndex >= 0 && selectedProjectIndex < static_cast<int>(discoveredProjectPaths.size())) {
					previewLabel = std::filesystem::path(discoveredProjectPaths[selectedProjectIndex]).parent_path().filename().string();
				}
				ImGui::SetNextItemWidth(-1);
				if (ImGui::BeginCombo("##ProjectList", previewLabel.c_str())) {
					for (int i = 0; i < static_cast<int>(discoveredProjectPaths.size()); ++i) {
						const std::string& path = discoveredProjectPaths[i];
						std::string label = std::filesystem::path(path).parent_path().filename().string();
						bool isSelected = (selectedProjectIndex == i);
						if (ImGui::Selectable(label.c_str(), isSelected)) {
							selectedProjectIndex = i;
							auto project = ProjectSerializer::LoadFromFile(path);
							if (project) {
								application.DismissProjectScreen();
								application.SetProject(std::move(project));
								application.OnProjectChosen(application.GetProject());
							}
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			ImGui::SetNextItemWidth(-1);
			ImGui::InputTextWithHint("##OpenPath", "Or paste path to .project file", openProjectPath, kPathBufferSize);

			if (ImGui::Button("Open", ImVec2(-1, 0))) {
				std::string path(openProjectPath);
				if (!path.empty()) {
					auto project = ProjectSerializer::LoadFromFile(path);
					if (project) {
						application.DismissProjectScreen();
						application.SetProject(std::move(project));
						application.OnProjectChosen(application.GetProject());
					}
				}
			}

			ImGui::Spacing();
			ImGui::TextDisabled("(Select from dropdown or paste the full path to your .project file)");
		}
		ImGui::End();
	}

}
