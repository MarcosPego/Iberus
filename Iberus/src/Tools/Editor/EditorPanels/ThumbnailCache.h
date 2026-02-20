#pragma once

#include "Core.h"

#include <string>
#include <vector>

namespace Iberus {

	enum class AssetIconType { Folder, Mesh, Shader, Image, File };

	/// Caches thumbnails for asset browser. PNG/JPG show image preview; folders/meshes/shaders use proper icons.
	class IBERUS_API ThumbnailCache {
	public:
		ThumbnailCache();
		~ThumbnailCache();

		/// Returns ImTextureID for ImGui::Image. Size 64x64. Invalidates on path change.
		void* GetTextureId(const std::string& path, bool isDirectory);
		void Invalidate(const std::string& path);
		void Clear();

		static constexpr int kThumbSize = 64;

	private:
		void CreateIconsIfNeeded();
		unsigned int GetIconForType(AssetIconType type);
		unsigned int GetOrCreateImageTexture(const std::string& path);
		AssetIconType GetIconTypeForPath(const std::string& path, bool isDirectory) const;

		struct CacheEntry {
			unsigned int textureId{ 0 };
			std::string path;
			bool isDirectory{ false };
		};
		std::vector<CacheEntry> cache;
		unsigned int folderIconId{ 0 };
		unsigned int meshIconId{ 0 };
		unsigned int shaderIconId{ 0 };
		unsigned int fileIconId{ 0 };
		bool iconsCreated{ false };
	};

}
