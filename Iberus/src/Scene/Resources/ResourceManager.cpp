#include "Enginepch.h"
#include "ResourceManager.h"

namespace Iberus {

	template<>
	Mesh* ResourceManager::InitializeResource(const std::string& id) {


		return nullptr;
	}

	template<>
	Shader* ResourceManager::InitializeResource(const std::string& id) {
		return nullptr;
	}

	/*template<>
	Texture* CreateResource(const std::string& id) {
		return nullptr;
	}*/


}