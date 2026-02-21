#include "Enginepch.h"
#include "AssetInspectorPanel.h"
#include "Editor.h"
#include "Engine.h"
#include "FileSystem.h"
#include "Material.h"
#include "MaterialSerializer.h"
#include "ResourceManager.h"
#include "Shader.h"

#include "imgui.h"

#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace Iberus {

	AssetInspectorPanel::AssetInspectorPanel(Editor& editor) : editor(editor) {
	}

	void AssetInspectorPanel::DrawFileInfo(const std::string& path) {
		std::filesystem::path p(path);
		if (!std::filesystem::exists(p)) {
			return;
		}

		ImGui::Text("Path: %s", path.c_str());
		ImGui::Spacing();

		if (std::filesystem::is_directory(p)) {
			ImGui::Text("Type: Directory");
		} else {
			ImGui::Text("Type: File");
			auto ext = p.extension().string();
			if (!ext.empty()) {
				ImGui::Text("Extension: %s", ext.c_str());
			}

			std::size_t size = FileSystem::GetRawFileSize(path);
			std::ostringstream oss;
			if (size < 1024) {
				oss << size << " B";
			} else if (size < 1024 * 1024) {
				oss << (size / 1024) << " KB";
			} else {
				oss << std::fixed << std::setprecision(2) << (size / (1024.0 * 1024.0)) << " MB";
			}
			ImGui::Text("Size: %s", oss.str().c_str());

#if __cplusplus >= 202002L
			std::error_code ec;
			auto lastWrite = std::filesystem::last_write_time(p, ec);
			if (!ec) {
				auto sysTime = std::chrono::clock_cast<std::chrono::system_clock>(lastWrite);
				auto timeT = std::chrono::system_clock::to_time_t(sysTime);
				std::tm tmBuf;
#ifdef _WIN32
				localtime_s(&tmBuf, &timeT);
#else
				localtime_r(&timeT, &tmBuf);
#endif
				std::ostringstream ts;
				ts << std::put_time(&tmBuf, "%Y-%m-%d %H:%M:%S");
				ImGui::Text("Modified: %s", ts.str().c_str());
			}
#endif
		}
	}

	void AssetInspectorPanel::DrawMaterialEditor(const std::string& path) {
		if (cachedMaterialPath != path) {
			cachedMaterialPath = path;
			cachedMaterial.reset();
			auto* engine = Engine::Instance();
			cachedMaterial = MaterialSerializer::LoadFromFile(path, engine->GetResourceManager(), &engine->GetEngineProvider());
		}

		if (!cachedMaterial) {
			ImGui::Text("Failed to load material");
			ImGui::Spacing();
			DrawFileInfo(path);
			return;
		}

		Material& mat = *cachedMaterial;

		ImGui::Text("Material");
		ImGui::Separator();

		if (ImGui::ColorEdit4("Albedo Color##MaterialAlbedo", &mat.albedoColor.x)) {
		}

		ImGui::Separator();
		ImGui::Text("Emissive");
		if (ImGui::ColorEdit3("Emissive Color##MaterialEmissive", &mat.emissiveColor.x)) {
		}
		if (ImGui::SliderFloat("Emissive Intensity##MaterialEmissive", &mat.emissiveIntensity, 0.0f, 10.0f, "%.2f")) {
		}

		ImGui::Separator();
		ImGui::Text("Shader");
		char shaderBuf[256];
		snprintf(shaderBuf, sizeof(shaderBuf), "%s", mat.GetShaderId().c_str());
		if (ImGui::InputText("Shader ID##MaterialShader", shaderBuf, sizeof(shaderBuf))) {
			std::string newId(shaderBuf);
			if (!newId.empty()) {
				auto* engine = Engine::Instance();
				Shader* shader = engine->GetResourceManager().GetOrCreateResource<Shader>(newId, &engine->GetEngineProvider());
				if (shader) {
					mat.SetShader(shader);
				}
			}
		}

		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::Button("Save##MaterialSave")) {
			MaterialSerializer::SaveToFile(mat, path);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Text("File Info");
		ImGui::Separator();
		DrawFileInfo(path);
	}

	void AssetInspectorPanel::DrawUnknownAsset(const std::string& path) {
		ImGui::Text("No specialized editor for this asset type.");
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Text("File Info");
		ImGui::Separator();
		DrawFileInfo(path);
	}

	void AssetInspectorPanel::OnDraw(IGUIContext& gui) {
		if (!gui.BeginWindow("Asset Inspector")) {
			gui.EndWindow();
			return;
		}

		std::string path = editor.GetSelectedAssetPath();
		if (path.empty()) {
			cachedMaterialPath.clear();
			cachedMaterial.reset();
			gui.Text("Select a file in the File System panel");
			gui.EndWindow();
			return;
		}

		std::filesystem::path p(path);
		if (!std::filesystem::exists(p)) {
			gui.Text("File not found");
			gui.EndWindow();
			return;
		}

		std::string ext = p.extension().string();
		if (ext == ".mat") {
			DrawMaterialEditor(path);
		} else {
			cachedMaterialPath.clear();
			cachedMaterial.reset();
			DrawUnknownAsset(path);
		}

		gui.EndWindow();
	}

}
