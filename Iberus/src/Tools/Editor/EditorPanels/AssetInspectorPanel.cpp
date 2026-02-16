#include "Enginepch.h"
#include "AssetInspectorPanel.h"
#include "Editor.h"
#include "FileSystem.h"

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
			return;
		}

		std::string path = editor.GetSelectedAssetPath();
		if (path.empty()) {
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
			ImGui::Text("Material asset (.mat) - editor coming in Phase 5");
			ImGui::Spacing();
			DrawFileInfo(path);
		} else {
			DrawUnknownAsset(path);
		}

		gui.EndWindow();
	}

}
