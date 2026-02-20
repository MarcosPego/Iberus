#include "Enginepch.h"
#include "ThumbnailCache.h"
#include "FileSystem.h"

#include <stb_image.h>

#include <filesystem>
#include <algorithm>
#include <cmath>

namespace Iberus {

	static constexpr int kIconSize = 64;

	static unsigned int CreateTextureFromRGBA(const unsigned char* data, int w, int h) {
		unsigned int texId;
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		GLenum format = (h > 0 && w > 0) ? GL_RGBA : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
		return texId;
	}

	static void FillRect(std::vector<unsigned char>& pixels, int px, int py, int w, int h, unsigned char r, unsigned char g, unsigned char b) {
		for (int y = py; y < py + h && y < kIconSize; ++y) {
			for (int x = px; x < px + w && x < kIconSize; ++x) {
				if (x >= 0 && y >= 0) {
					int i = (y * kIconSize + x) * 4;
					pixels[i] = r;
					pixels[i + 1] = g;
					pixels[i + 2] = b;
					pixels[i + 3] = 255;
				}
			}
		}
	}

	static void DrawLine(std::vector<unsigned char>& pixels, int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b) {
		int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx + dy;
		while (true) {
			if (x0 >= 0 && x0 < kIconSize && y0 >= 0 && y0 < kIconSize) {
				int i = (y0 * kIconSize + x0) * 4;
				pixels[i] = r;
				pixels[i + 1] = g;
				pixels[i + 2] = b;
				pixels[i + 3] = 255;
			}
			if (x0 == x1 && y0 == y1) {
				break;
			}
			int e2 = 2 * err;
			if (e2 >= dy) {
				err += dy;
				x0 += sx;
			}
			if (e2 <= dx) {
				err += dx;
				y0 += sy;
			}
		}
	}

	static unsigned int CreateFolderIcon() {
		std::vector<unsigned char> pixels(kIconSize * kIconSize * 4, 0);
		// Background: light yellow/tan
		for (int i = 0; i < kIconSize * kIconSize * 4; i += 4) {
			pixels[i] = 250;
			pixels[i + 1] = 220;
			pixels[i + 2] = 130;
			pixels[i + 3] = 255;
		}
		// Folder tab (darker)
		FillRect(pixels, 8, 8, 24, 12, 230, 190, 90);
		// Folder body
		FillRect(pixels, 8, 20, 48, 36, 250, 220, 130);
		// Dark outline
		FillRect(pixels, 8, 8, 48, 4, 200, 150, 50);
		FillRect(pixels, 8, 12, 4, 44, 200, 150, 50);
		FillRect(pixels, 52, 20, 4, 36, 200, 150, 50);
		FillRect(pixels, 8, 54, 48, 4, 200, 150, 50);
		return CreateTextureFromRGBA(pixels.data(), kIconSize, kIconSize);
	}

	static unsigned int CreateCubeIcon() {
		std::vector<unsigned char> pixels(kIconSize * kIconSize * 4, 0);
		for (int i = 0; i < kIconSize * kIconSize * 4; i += 4) {
			pixels[i] = 70;
			pixels[i + 1] = 70;
			pixels[i + 2] = 80;
			pixels[i + 3] = 255;
		}
		// Isometric cube: 3 visible faces
		// Top face (lighter)
		int cx = 32, cy = 18;
		int sx = 20, sy = 10;
		// Top face vertices (diamond)
		DrawLine(pixels, cx, cy - sy, cx + sx, cy, 180, 180, 200);
		DrawLine(pixels, cx + sx, cy, cx, cy + sy, 180, 180, 200);
		DrawLine(pixels, cx, cy + sy, cx - sx, cy, 180, 180, 200);
		DrawLine(pixels, cx - sx, cy, cx, cy - sy, 180, 180, 200);
		// Left face (darker)
		DrawLine(pixels, cx - sx, cy, cx - sx + 8, cy + 24, 120, 120, 140);
		DrawLine(pixels, cx, cy + sy, cx + 8, cy + sy + 24, 120, 120, 140);
		DrawLine(pixels, cx - sx, cy, cx, cy + sy, 120, 120, 140);
		DrawLine(pixels, cx - sx + 8, cy + 24, cx + 8, cy + sy + 24, 120, 120, 140);
		// Right face (medium)
		DrawLine(pixels, cx + sx, cy, cx + sx + 8, cy + 24, 140, 140, 160);
		DrawLine(pixels, cx, cy + sy, cx + 8, cy + sy + 24, 140, 140, 160);
		DrawLine(pixels, cx + sx, cy, cx, cy + sy, 140, 140, 160);
		DrawLine(pixels, cx + sx + 8, cy + 24, cx + 8, cy + sy + 24, 140, 140, 160);
		return CreateTextureFromRGBA(pixels.data(), kIconSize, kIconSize);
	}

	static unsigned int CreateShaderIcon() {
		std::vector<unsigned char> pixels(kIconSize * kIconSize * 4, 0);
		for (int i = 0; i < kIconSize * kIconSize * 4; i += 4) {
			pixels[i] = 50;
			pixels[i + 1] = 50;
			pixels[i + 2] = 60;
			pixels[i + 3] = 255;
		}
		// Code brackets { } in purple/blue
		unsigned char cr = 160, cg = 120, cb = 220;
		int cx = 32;
		// Left brace {
		DrawLine(pixels, cx - 16, 16, cx - 16, 48, cr, cg, cb);
		DrawLine(pixels, cx - 16, 16, cx - 8, 24, cr, cg, cb);
		DrawLine(pixels, cx - 16, 48, cx - 8, 40, cr, cg, cb);
		DrawLine(pixels, cx - 8, 24, cx - 8, 40, cr, cg, cb);
		// Right brace }
		DrawLine(pixels, cx + 16, 16, cx + 16, 48, cr, cg, cb);
		DrawLine(pixels, cx + 16, 16, cx + 8, 24, cr, cg, cb);
		DrawLine(pixels, cx + 16, 48, cx + 8, 40, cr, cg, cb);
		DrawLine(pixels, cx + 8, 24, cx + 8, 40, cr, cg, cb);
		return CreateTextureFromRGBA(pixels.data(), kIconSize, kIconSize);
	}

	static unsigned int CreateFileIcon() {
		std::vector<unsigned char> pixels(kIconSize * kIconSize * 4, 0);
		for (int i = 0; i < kIconSize * kIconSize * 4; i += 4) {
			pixels[i] = 220;
			pixels[i + 1] = 220;
			pixels[i + 2] = 230;
			pixels[i + 3] = 255;
		}
		// Document shape with folded corner
		FillRect(pixels, 12, 8, 40, 48, 240, 240, 245);
		FillRect(pixels, 12, 8, 4, 48, 180, 180, 190);
		FillRect(pixels, 48, 8, 4, 20, 180, 180, 190);
		FillRect(pixels, 44, 24, 12, 4, 200, 200, 210);
		return CreateTextureFromRGBA(pixels.data(), kIconSize, kIconSize);
	}

	void ThumbnailCache::CreateIconsIfNeeded() {
		if (iconsCreated) {
			return;
		}
		folderIconId = CreateFolderIcon();
		meshIconId = CreateCubeIcon();
		shaderIconId = CreateShaderIcon();
		fileIconId = CreateFileIcon();
		iconsCreated = true;
	}

	ThumbnailCache::ThumbnailCache() {
		CreateIconsIfNeeded();
	}

	ThumbnailCache::~ThumbnailCache() {
		Clear();
		if (folderIconId) {
			glDeleteTextures(1, &folderIconId);
			folderIconId = 0;
		}
		if (meshIconId) {
			glDeleteTextures(1, &meshIconId);
			meshIconId = 0;
		}
		if (shaderIconId) {
			glDeleteTextures(1, &shaderIconId);
			shaderIconId = 0;
		}
		if (fileIconId) {
			glDeleteTextures(1, &fileIconId);
			fileIconId = 0;
		}
	}

	static bool IsSystemIconId(unsigned int id, unsigned int folderId, unsigned int meshId, unsigned int shaderId, unsigned int fileId) {
		return id == folderId || id == meshId || id == shaderId || id == fileId;
	}

	void ThumbnailCache::Clear() {
		for (auto& e : cache) {
			if (e.textureId && !IsSystemIconId(e.textureId, folderIconId, meshIconId, shaderIconId, fileIconId)) {
				glDeleteTextures(1, &e.textureId);
			}
		}
		cache.clear();
	}

	void ThumbnailCache::Invalidate(const std::string& path) {
		cache.erase(std::remove_if(cache.begin(), cache.end(),
			[this, &path](const CacheEntry& e) {
				if (e.path == path && e.textureId != 0 && !IsSystemIconId(e.textureId, folderIconId, meshIconId, shaderIconId, fileIconId)) {
					glDeleteTextures(1, &e.textureId);
					return true;
				}
				return false;
			}), cache.end());
	}

	AssetIconType ThumbnailCache::GetIconTypeForPath(const std::string& path, bool isDirectory) const {
		if (isDirectory) {
			return AssetIconType::Folder;
		}
		std::string ext = std::filesystem::path(path).extension().string();
		for (auto& c : ext) {
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
		if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb" || ext == ".mesh") {
			return AssetIconType::Mesh;
		}
		if (ext == ".vert" || ext == ".frag" || ext == ".glsl" || ext == ".hlsl" || ext == ".vs" || ext == ".fs" || ext == ".shader") {
			return AssetIconType::Shader;
		}
		if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga") {
			return AssetIconType::Image;
		}
		return AssetIconType::File;
	}

	unsigned int ThumbnailCache::GetIconForType(AssetIconType type) {
		CreateIconsIfNeeded();
		switch (type) {
		case AssetIconType::Folder: return folderIconId;
		case AssetIconType::Mesh: return meshIconId;
		case AssetIconType::Shader: return shaderIconId;
		case AssetIconType::File: return fileIconId;
		case AssetIconType::Image: return 0;
		default: return fileIconId;
		}
	}

	unsigned int ThumbnailCache::GetOrCreateImageTexture(const std::string& path) {
		auto it = std::find_if(cache.begin(), cache.end(), [&path](const CacheEntry& e) { return e.path == path && !e.isDirectory; });
		if (it != cache.end()) {
			return it->textureId;
		}

		std::string ext = std::filesystem::path(path).extension().string();
		for (auto& c : ext) {
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
		if (ext != ".png" && ext != ".jpg" && ext != ".jpeg" && ext != ".bmp" && ext != ".tga") {
			return 0;
		}

		Buffer buf = FileSystem::GetRawFileBuffer(path);
		if (buf.Invalid()) {
			return 0;
		}

		int w, h, ch;
		unsigned char* data = stbi_load_from_memory(
			static_cast<unsigned char*>(buf.GetData()),
			static_cast<int>(buf.GetSize()),
			&w, &h, &ch, 4);
		if (!data) {
			return 0;
		}

		unsigned int texId = CreateTextureFromRGBA(data, w, h);
		stbi_image_free(data);

		CacheEntry e;
		e.textureId = texId;
		e.path = path;
		e.isDirectory = false;
		cache.push_back(e);

		return texId;
	}

	void* ThumbnailCache::GetTextureId(const std::string& path, bool isDirectory) {
		AssetIconType iconType = GetIconTypeForPath(path, isDirectory);

		if (iconType == AssetIconType::Image) {
			unsigned int id = GetOrCreateImageTexture(path);
			if (id != 0) {
				return reinterpret_cast<void*>(static_cast<intptr_t>(id));
			}
			iconType = AssetIconType::File;
		}

		unsigned int iconId = GetIconForType(iconType);
		return reinterpret_cast<void*>(static_cast<intptr_t>(iconId));
	}

}
