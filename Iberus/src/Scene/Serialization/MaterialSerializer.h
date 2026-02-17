#pragma once

#include "Core.h"
#include "Buffer.h"

#include <memory>
#include <string>

namespace Iberus {

	class Material;
	class ResourceManager;
	class IProvider;

	class IBERUS_API MaterialSerializer {
	public:
		static Buffer Serialize(const Material& material);

		/// Deserialize from Buffer. Uses resourceManager and provider to resolve shader and texture paths.
		static std::unique_ptr<Material> Deserialize(const Buffer& buffer, ResourceManager& resourceManager, IProvider* provider);

		static bool SaveToFile(const Material& material, const std::string& path);
		static std::unique_ptr<Material> LoadFromFile(const std::string& path, ResourceManager& resourceManager, IProvider* provider);
	};

}
