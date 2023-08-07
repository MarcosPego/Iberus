#pragma once

#include "IProvider.h"

namespace Iberus {

	class FileSystemProvider : public IProvider {

		Buffer GetRawFileBuffer(const std::string& filename) override;

	private:
		std::string workingDir;

	};
}


