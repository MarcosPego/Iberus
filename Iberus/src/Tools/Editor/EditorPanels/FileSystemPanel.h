#pragma once

#include "IGUIContext.h"

#include <memory>
#include <string>

namespace Iberus {

	class Editor;
	class FileWatcher;
	class ThumbnailCache;

	class FileSystemPanel {
	public:
		explicit FileSystemPanel(Editor& editor);
		~FileSystemPanel();

		void OnDraw(IGUIContext& gui);

	private:
		void DrawFolderTree(const std::string& path, int depth);
		void DrawContentGrid(const std::string& path);
		void DrawBreadcrumb(const std::string& path);
		void DrawToolbar();
		std::string GetAssetsPath() const;
		void OnFileSystemChanged(const std::string& path);
		bool PassesFilter(const std::string& name) const;

		Editor& editor;
		std::string currentPath;
		std::string searchFilter;
		bool contentDirty{ true };
		bool viewGrid{ true };
		float iconSize{ 64.f };

		std::unique_ptr<FileWatcher> fileWatcher;
		std::unique_ptr<ThumbnailCache> thumbnailCache;
	};

}
