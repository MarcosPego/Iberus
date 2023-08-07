#pragma once

#include "IProvider.h"

namespace Iberus {

	class FileSystemProvider : public IProvider {
	public:
		Buffer GetRawFileBuffer(const std::string& filename) override;

		void SetWorkingDir(const std::string& newWorkingDir) {
			workingDir = newWorkingDir;
		}

	private:
		std::string workingDir;

	};
}


