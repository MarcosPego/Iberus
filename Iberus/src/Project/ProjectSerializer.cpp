#include "Enginepch.h"
#include "ProjectSerializer.h"
#include "FileSystem.h"
#include "JsonValue.h"

#include <fstream>
#include <filesystem>

namespace Iberus {

	Buffer ProjectSerializer::Serialize(const Project& project) {
		JsonValue root = JsonValue::Object();
		root.Set("version", project.GetVersion());
		root.Set("name", project.GetName());
		root.Set("rootPath", project.GetRootPath());
		root.Set("currentScenePath", project.GetCurrentScenePath());

		JsonValue recentArr = JsonValue::Array();
		for (size_t i = 0; i < project.GetRecentScenes().size(); ++i) {
			recentArr.Set(i, JsonValue::String(project.GetRecentScenes()[i]));
		}
		root.Set("recentScenes", recentArr);

		return root.ToBuffer(true);
	}

	std::unique_ptr<Project> ProjectSerializer::Deserialize(const Buffer& buffer) {
		JsonValue root = JsonValue::Parse(buffer);
		if (!root.IsValid() || !root.IsObject()) {
			return nullptr;
		}

		auto project = std::make_unique<Project>();
		project->SetVersion(root.Contains("version") ? root["version"].AsInt() : 1);
		project->SetName(root.Contains("name") ? root["name"].AsString() : "Untitled");
		project->SetRootPath(root.Contains("rootPath") ? root["rootPath"].AsString() : "");
		project->SetCurrentScenePath(root.Contains("currentScenePath") ? root["currentScenePath"].AsString() : "");

		if (root.Contains("recentScenes") && root["recentScenes"].IsArray()) {
			JsonValue arr = root["recentScenes"];
			std::vector<std::string> recent;
			for (size_t i = 0; i < arr.Size(); ++i) {
				if (arr[i].IsString()) {
					recent.push_back(arr[i].AsString());
				}
			}
			project->SetRecentScenes(std::move(recent));
		}

		return project;
	}

	bool ProjectSerializer::SaveToFile(const Project& project, const std::string& path) {
		Buffer buf = Serialize(project);
		if (buf.Invalid()) {
			return false;
		}
		std::ofstream f(path, std::ios::binary);
		if (!f) {
			return false;
		}
		f.write(reinterpret_cast<const char*>(buf.GetData()), static_cast<std::streamsize>(buf.GetSize()));
		return f.good();
	}

	std::unique_ptr<Project> ProjectSerializer::LoadFromFile(const std::string& path) {
		Buffer buf = FileSystem::GetRawFileBuffer(path);
		if (buf.Invalid()) {
			return nullptr;
		}
		auto project = Deserialize(buf);
		if (!project) {
			return nullptr;
		}
		std::string rootPath = project->GetRootPath();
		if (rootPath.empty() || rootPath == ".") {
			std::filesystem::path projectPath(path);
			project->SetRootPath(projectPath.parent_path().string());
		}
		return project;
	}

}
