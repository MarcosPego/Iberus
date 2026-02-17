#pragma once

#include "Core.h"
#include "Project.h"
#include "Buffer.h"

#include <memory>
#include <string>

namespace Iberus {

	class IBERUS_API ProjectSerializer {
	public:
		static Buffer Serialize(const Project& project);
		static std::unique_ptr<Project> Deserialize(const Buffer& buffer);

		static bool SaveToFile(const Project& project, const std::string& path);
		static std::unique_ptr<Project> LoadFromFile(const std::string& path);
	};

}
