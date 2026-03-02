#include "Enginepch.h"
#include "FileSystemPanel.h"
#include "Editor.h"
#include "FileSystem.h"
#include "FileWatcher.h"
#include "ThumbnailCache.h"
#include "IconsFontAwesome6.h"

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

	static const char* GetIconForAssetType(AssetIconType type) {
		switch (type) {
		case AssetIconType::Folder: return ICON_FA_FOLDER;
		case AssetIconType::Mesh: return ICON_FA_CUBE;
		case AssetIconType::Shader: return ICON_FA_CODE;
		case AssetIconType::Scene: return ICON_FA_FILM;
		case AssetIconType::Material: return ICON_FA_SWATCHBOOK;
		case AssetIconType::Prefab: return ICON_FA_OBJECT_GROUP;
		case AssetIconType::Image: return ICON_FA_IMAGE;
		case AssetIconType::File:
		default: return ICON_FA_FILE;
		}
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
		if (ImGui::Button(ICON_FA_FOLDER " Project Root##ProjectRoot")) {
			currentPath = GetAssetsPath();
			contentDirty = true;
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		char filterBuf[128];
		snprintf(filterBuf, sizeof(filterBuf), "%s", searchFilter.c_str());
		if (ImGui::InputTextWithHint("##Search", ICON_FA_MAGNIFYING_GLASS " Search...", filterBuf, sizeof(filterBuf))) {
			searchFilter = filterBuf;
			contentDirty = true;
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::SliderFloat("##IconSize", &iconSize, 32.f, 128.f, "%.0f");
		ImGui::SameLine();
		if (ImGui::Button(viewGrid ? ICON_FA_LIST " List##ViewList" : ICON_FA_GRIP " Grid##ViewGrid")) {
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

		ImGui::TextUnformatted(ICON_FA_FOLDER " Assets");
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

	static bool FolderHasContents(const std::string& path) {
		auto entries = FileSystem::ListDirectory(path);
		return !entries.empty();
	}

	void FileSystemPanel::DrawFolderTreeContents(const std::string& path, int depth) {
		auto entries = FileSystem::ListDirectory(path);
		// Subfolders first (expandable)
		for (const auto& e : entries) {
			if (!e.isDirectory) {
				continue;
			}
			if (!PassesFilter(e.name)) {
				continue;
			}
			std::string fullPath = (std::filesystem::path(path) / e.name).string();
			bool selected = (currentPath == fullPath);
			bool hasContents = FolderHasContents(fullPath);
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			if (selected) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			if (!hasContents) {
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			char treeBuf[512];
			snprintf(treeBuf, sizeof(treeBuf), "%s %s##FolderTree_%s", ICON_FA_FOLDER, e.name.c_str(), fullPath.c_str());
			bool open = ImGui::TreeNodeEx(treeBuf, flags);
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
				currentPath = fullPath;
				contentDirty = true;
			}
			if (open) {
				DrawFolderTreeContents(fullPath, depth + 1);
				ImGui::TreePop();
			}
		}
		// Files as leaf items with icon + name (TreeNodeEx for alignment with folders)
		for (const auto& e : entries) {
			if (e.isDirectory) {
				continue;
			}
			if (!PassesFilter(e.name)) {
				continue;
			}
			std::string fullPath = (std::filesystem::path(path) / e.name).string();
			bool selected = (editor.GetSelectedAssetPath() == fullPath);
			AssetIconType iconType = thumbnailCache->GetIconTypeForPath(fullPath, false);
			char treeBuf[512];
			snprintf(treeBuf, sizeof(treeBuf), "%s %s##FolderTreeFile_%s", GetIconForAssetType(iconType), e.name.c_str(), fullPath.c_str());
			ImGuiTreeNodeFlags fileFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			if (selected) {
				fileFlags |= ImGuiTreeNodeFlags_Selected;
			}
			ImGui::TreeNodeEx(treeBuf, fileFlags);
			if (ImGui::IsItemClicked()) {
				editor.SetSelectedAssetPath(fullPath);
				currentPath = path;
				contentDirty = true;
			}
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				ImGui::SetDragDropPayload("IBERUS_ASSET_PATH", fullPath.c_str(), fullPath.size() + 1);
				ImGui::TextUnformatted(e.name.c_str());
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Select##SelectAssetTree")) {
					editor.SetSelectedAssetPath(fullPath);
				}
#ifdef _WIN32
				if (ImGui::MenuItem("Show in Explorer##ShowInExplorerTree")) {
					std::string pathToOpen = std::filesystem::path(fullPath).parent_path().string();
					std::wstring wpath(pathToOpen.begin(), pathToOpen.end());
					ShellExecuteW(nullptr, L"explore", wpath.c_str(), nullptr, nullptr, SW_SHOW);
				}
#endif
				ImGui::EndPopup();
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

				AssetIconType iconType = thumbnailCache->GetIconTypeForPath(fullPath, e.isDirectory);
				bool useImage = (iconType == AssetIconType::Image);
				void* texId = thumbnailCache->GetTextureId(fullPath, e.isDirectory);
				useImage = useImage && (texId != nullptr);

				bool clicked = false;
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
				if (useImage) {
					ImVec4 selTint = selected ? ImGui::GetStyleColorVec4(ImGuiCol_Header) : ImVec4(0, 0, 0, 0);
					clicked = ImGui::ImageButton("##AssetIcon", ImTextureRef(static_cast<ImTextureID>(reinterpret_cast<intptr_t>(texId))), ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0), selTint);
				} else {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.15f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.25f));
					if (selected) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Header));
					}
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
					float iconScale = std::clamp(iconSize / 24.0f, 1.5f, 4.0f);
					ImGui::SetWindowFontScale(iconScale);
					clicked = ImGui::Button(GetIconForAssetType(iconType), ImVec2(iconSize, iconSize));
					ImGui::SetWindowFontScale(1.0f);
					ImGui::PopStyleVar();
					ImGui::PopStyleColor(selected ? 4 : 3);
				}
				ImGui::PopStyleVar();
				if (clicked) {
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
				AssetIconType iconType = thumbnailCache->GetIconTypeForPath(fullPath, e.isDirectory);
				std::string label = std::string(GetIconForAssetType(iconType)) + " " + e.name;
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

	void FileSystemPanel::OnDraw(IGUIContext& gui, bool* p_open) {
		if (gui.BeginWindow("Project", p_open)) {
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
				ImGui::BeginChild("FolderTree##FolderTreeChild", ImVec2(leftWidth, 0), ImGuiChildFlags_ResizeX);

				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				if (ImGui::TreeNodeEx(ICON_FA_FOLDER " Assets##FolderTreeRoot", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) {
					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
						currentPath = assetsPath;
						contentDirty = true;
					}
					DrawFolderTreeContents(assetsPath, 0);
					ImGui::TreePop();
				}

				ImGui::EndChild();

				// Draw visible resize bar on the right edge of the folder tree
				ImVec2 treeMin = ImGui::GetItemRectMin();
				ImVec2 treeMax = ImGui::GetItemRectMax();
				float resizeBarX = treeMax.x;
				float barWidth = 2.0f;
				ImGui::GetWindowDrawList()->AddRectFilled(
					ImVec2(resizeBarX - barWidth * 0.5f, treeMin.y),
					ImVec2(resizeBarX + barWidth * 0.5f, treeMax.y),
					ImGui::GetColorU32(ImGuiCol_Separator));

				ImGui::SameLine();

				ImGui::BeginChild("ContentArea##ContentAreaChild", ImVec2(0, 0), ImGuiChildFlags_None);

				std::filesystem::path p(currentPath);
				if (p.has_parent_path()) {
					std::string parentPath = p.parent_path().string();
					if (parentPath.size() >= assetsPath.size() && parentPath.substr(0, assetsPath.size()) == assetsPath) {
						if (ImGui::Selectable(ICON_FA_ANGLES_LEFT " Parent folder##ParentFolder")) {
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
