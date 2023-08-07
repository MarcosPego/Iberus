#include "Enginepch.h"
#include "FileSystemProvider.h"

#include "FileSystem.h"

namespace Iberus {

	Buffer FileSystemProvider::GetRawFileBuffer(const std::string& filename) {
		// TODO see if path is absolute or rel
		Log::GetClientLogger()->info(workingDir + "/" + filename);
		return FileSystem::GetRawFileBuffer(workingDir + "/" + filename);
	}

}