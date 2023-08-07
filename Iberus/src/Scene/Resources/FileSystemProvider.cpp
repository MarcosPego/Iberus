#include "Enginepch.h"
#include "FileSystemProvider.h"

#include "FileSystem.h"

namespace Iberus {

	Buffer FileSystemProvider::GetRawFileBuffer(const std::string& filename) {
		return FileSystem::GetRawFileBuffer(filename);
	}

}