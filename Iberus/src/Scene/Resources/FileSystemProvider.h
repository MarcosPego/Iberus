#pragma once

#include "IProvider.h"

#include <string>

namespace Iberus {

	class FileSystemProvider : public IProvider {
	public:
		Buffer GetRawFileBuffer(const std::string& filename) override;

		void SetWorkingDir(const std::string& newWorkingDir) {
			workingDir = newWorkingDir;
		}

		/// Returns path from which resources are loaded (working dir).
		const std::string& GetWorkingDir() const { return workingDir; }

	private:
		std::string workingDir;
	};
}


