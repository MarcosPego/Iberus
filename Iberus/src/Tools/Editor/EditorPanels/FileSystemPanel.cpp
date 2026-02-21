#include "Enginepch.h"
#include "FileSystemPanel.h"
#include "Editor.h"
#include "FileSystem.h"
#include "FileWatcher.h"
#include "ThumbnailCache.h"

#include "imgui.h"

#ifdef _WIN32
#include <Windows.h>
#include <Shellapi.h>
#endif

#include <filesystem>
#include <algorithm>

namespace Iberus {

	FileSystemPanel::FileSystemPanel(Editor& editor)
		: editor(editor) {
		currentPath = GetAssetsPath();
		thumbnailCache = std::make_unique<ThumbnailCache>();

		std::string assetsPath = GetAssetsPath();
		if (std::filesystem::exists(assetsPath) && std::filesystem::is_directory(assetsPath)) {
			fileWatcher = std::make_unique<FileWatcher>(assetsPath);
			fileWatcher->SetCallback([this](const std::string& path) {
				contentDirty = true;
				thumbnailCache->Invalidate(path);
			});
			fileWatcher->Start();
		}
	}

	FileSystemPanel::~FileSystemPanel() {
		if (fileWatcher) {
			fileWatcher->Stop();
		}
	}

	std::string FileSystemPanel::GetAssetsPath() const {
		return FileSystem::GetAssetsPath();
	}

	void FileSystemPanel::OnFileSystemChanged(const std::string& path) {
		contentDirty = true;
		thumbnailCache->Invalidate(path);
	}

	bool FileSystemPanel::PassesFilter(const std::string& name) const {
		if (searchFilter.empty()) {
			return true;
		}
		std::string lowerName = name;
		std::string lowerFilter = searchFilter;
		std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return lowerName.find(lowerFilter) != std::string::npos;
	}

	void FileSystemPanel::DrawToolbar() {
		if (ImGui::Button("Project Root##ProjectRoot")) {
			currentPath = GetAssetsPath();
			contentDirty = true;
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		char filterBuf[128];
		snprintf(filterBuf, sizeof(filterBuf), "%s", searchFilter.c_str());
		if (ImGui::InputTextWithHint("##Search", "Search...", filterBuf, sizeof(filterBuf))) {
			searchFilter = filterBuf;
			contentDirty = true;
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::SliderFloat("##IconSize", &iconSize, 32.f, 128.f, "%.0f");
		ImGui::SameLine();
		if (ImGui::Button(viewGrid ? "List##ViewList" : "Grid##ViewGrid")) {
			viewGrid = !viewGrid;
		}
	}

	void FileSystemPanel::DrawBreadcrumb(const std::string& path) {
		std::string assetsPath = GetAssetsPath();
		std::string rel = path;
		if (path.size() >= assetsPath.size() && path.substr(0, assetsPath.size()) == assetsPath) {
			rel = path.substr(assetsPath.size());
			if (!rel.empty() && (rel[0] == '/' || rel[0] == '\\')) {
				rel = rel.substr(1);
			}
		}
		if (rel.empty()) {
			rel = "Assets";
		}

		ImGui::TextUnformatted("Assets");
		std::string acc = assetsPath;
		for (size_t i = 0; i < rel.size(); ) {
			size_t sep = rel.find_first_of("/\\", i);
			if (sep == std::string::npos) {
				sep = rel.size();
			}
			std::string part = rel.substr(i, sep - i);
			if (!part.empty()) {
				ImGui::SameLine();
				ImGui::TextUnformatted(" / ");
				ImGui::SameLine();
				acc = (std::filesystem::path(acc) / part).string();
				char bcBuf[512];
				snprintf(bcBuf, sizeof(bcBuf), "%s##Breadcrumb_%s", part.c_str(), acc.c_str());
				if (ImGui::SmallButton(bcBuf)) {
					currentPath = acc;
					contentDirty = true;
				}
			}
			i = sep;
			if (sep < rel.size()) {
				i++;
			}
		}
	}

	void FileSystemPanel::DrawFolderTree(const std::string& path, int depth) {
		auto entries = FileSystem::ListDirectory(path);
		for (const auto& e : entries) {
			if (!e.isDirectory) {
				continue;
			}
			if (!PassesFilter(e.name)) {
				continue;
			}

			std::string fullPath = (std::filesystem::path(path) / e.name).string();
			bool selected = (currentPath == fullPath);
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			if (selected) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			char treeBuf[512];
			snprintf(treeBuf, sizeof(treeBuf), "%s##FolderTree_%s", e.name.c_str(), fullPath.c_str());
			bool open = ImGui::TreeNodeEx(treeBuf, flags);
			if (ImGui::IsItemClicked()) {
				currentPath = fullPath;
				contentDirty = true;
			}
			if (open) {
				DrawFolderTree(fullPath, depth + 1);
				ImGui::TreePop();
			}
		}
	}

	void FileSystemPanel::DrawContentGrid(const std::string& path) {
		auto entries = FileSystem::ListDirectory(path);
		float panelW = ImGui::GetContentRegionAvail().x;
		int cols = viewGrid ? std::max(1, static_cast<int>(panelW / (iconSize + 24))) : 1;

		int col = 0;
		for (const auto& e : entries) {
			if (!PassesFilter(e.name)) {
				continue;
			}

			std::string fullPath = (std::filesystem::path(path) / e.name).string();
			bool selected = (editor.GetSelectedAssetPath() == fullPath);

			if (viewGrid) {
				if (col > 0) {
					ImGui::SameLine();
				}
				ImGui::BeginGroup();
				ImGui::PushID(fullPath.c_str());

				void* texId = thumbnailCache->GetTextureId(fullPath, e.isDirectory);
				if (ImGui::ImageButton("##AssetIcon", ImTextureRef(static_cast<ImTextureID>(reinterpret_cast<intptr_t>(texId))), ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0), selected ? ImVec4(0.3f, 0.5f, 0.8f, 1.f) : ImVec4(0, 0, 0, 0))) {
					if (e.isDirectory) {
						currentPath = fullPath;
						contentDirty = true;
					} else {
						editor.SetSelectedAssetPath(fullPath);
					}
				}
				if (!e.isDirectory && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
					ImGui::SetDragDropPayload("IBERUS_ASSET_PATH", fullPath.c_str(), fullPath.size() + 1);
					ImGui::TextUnformatted(e.name.c_str());
					ImGui::EndDragDropSource();
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
					if (e.isDirectory) {
						currentPath = fullPath;
						contentDirty = true;
					} else {
						editor.SetSelectedAssetPath(fullPath);
					}
				}
				if (ImGui::BeginPopupContextItem()) {
					if (ImGui::MenuItem("Select##SelectAsset")) {
						editor.SetSelectedAssetPath(fullPath);
					}
#ifdef _WIN32
					if (ImGui::MenuItem("Show in Explorer##ShowInExplorer")) {
						std::string pathToOpen = e.isDirectory ? fullPath : std::filesystem::path(fullPath).parent_path().string();
						std::wstring wpath(pathToOpen.begin(), pathToOpen.end());
						ShellExecuteW(nullptr, L"explore", wpath.c_str(), nullptr, nullptr, SW_SHOW);
					}
#endif
					ImGui::EndPopup();
				}

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
				std::string label = e.name;
				if (label.size() > 12) {
					label = label.substr(0, 10) + "..";
				}
				ImGui::TextWrapped("%s", label.c_str());
				ImGui::PopStyleColor();
				ImGui::PopID();
				ImGui::EndGroup();

				col++;
				if (col >= cols) {
					col = 0;
				}
			} else {
				ImGui::PushID(fullPath.c_str());
				std::string label = e.isDirectory ? "[D] " : "[F] ";
				label += e.name;
				label += "##ContentSelectable";
				if (ImGui::Selectable(label.c_str(), selected)) {
					if (e.isDirectory) {
						currentPath = fullPath;
						contentDirty = true;
					} else {
						editor.SetSelectedAssetPath(fullPath);
					}
				}
				if (!e.isDirectory && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
					ImGui::SetDragDropPayload("IBERUS_ASSET_PATH", fullPath.c_str(), fullPath.size() + 1);
					ImGui::TextUnformatted(e.name.c_str());
					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginPopupContextItem()) {
					if (ImGui::MenuItem("Select##SelectAssetList")) {
						editor.SetSelectedAssetPath(fullPath);
					}
#ifdef _WIN32
					if (ImGui::MenuItem("Show in Explorer##ShowInExplorerList")) {
						std::string pathToOpen = e.isDirectory ? fullPath : std::filesystem::path(fullPath).parent_path().string();
						std::wstring wpath(pathToOpen.begin(), pathToOpen.end());
						ShellExecuteW(nullptr, L"explore", wpath.c_str(), nullptr, nullptr, SW_SHOW);
					}
#endif
					ImGui::EndPopup();
				}
				ImGui::PopID();
			}
		}
	}

	void FileSystemPanel::OnDraw(IGUIContext& gui) {
		if (gui.BeginWindow("Project")) {
			if (!ImGui::IsWindowCollapsed()) {
				std::string assetsPath = GetAssetsPath();
				if (currentPath.empty() || currentPath.size() < assetsPath.size() || currentPath.substr(0, assetsPath.size()) != assetsPath) {
					currentPath = assetsPath;
				}

				DrawToolbar();
				ImGui::Separator();
				DrawBreadcrumb(currentPath);
				ImGui::Separator();

				float leftWidth = 180.f;
				ImGui::BeginChild("FolderTree##FolderTreeChild", ImVec2(leftWidth, 0), true);

				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				if (ImGui::TreeNodeEx("Assets##FolderTreeRoot", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf)) {
					if (ImGui::IsItemClicked()) {
						currentPath = assetsPath;
						contentDirty = true;
					}
					DrawFolderTree(assetsPath, 0);
					ImGui::TreePop();
				}

				ImGui::EndChild();

				ImGui::SameLine();

				ImGui::BeginChild("ContentArea##ContentAreaChild", ImVec2(0, 0), true);

				std::filesystem::path p(currentPath);
				if (p.has_parent_path()) {
					std::string parentPath = p.parent_path().string();
					if (parentPath.size() >= assetsPath.size() && parentPath.substr(0, assetsPath.size()) == assetsPath) {
						if (ImGui::Selectable("[..] Parent folder##ParentFolder")) {
							currentPath = parentPath;
							contentDirty = true;
						}
						ImGui::Separator();
					}
				}

				DrawContentGrid(currentPath);

				if (ImGui::BeginPopupContextWindow("ContentContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
					ImGui::TextUnformatted(currentPath.c_str());
					ImGui::Separator();
#ifdef _WIN32
					if (ImGui::MenuItem("Open in Explorer##ContentOpenExplorer")) {
						std::wstring wpath(currentPath.begin(), currentPath.end());
						ShellExecuteW(nullptr, L"explore", wpath.c_str(), nullptr, nullptr, SW_SHOW);
					}
#endif
					ImGui::EndPopup();
				}

				ImGui::EndChild();
			}
		}
		gui.EndWindow();
	}

}
