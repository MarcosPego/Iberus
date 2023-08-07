#include "Enginepch.h"
#include "ResourceManager.h"

namespace Iberus {

	/*template<>
	Mesh* ResourceManager::InitializeResource(const std::string& id, Buffer buffer) {
		auto* mesh = new Mesh(id, std::move(buffer));
		resources.emplace(id, mesh);

		return dynamic_cast<Mesh*>(resources.at(id).get());;
	}*/

	/*template<>
	Shader* ResourceManager::InitializeResource(const std::string& id) {
		return nullptr;
	}*/

	/*template<>
	Texture* CreateResource(const std::string& id) {
		return nullptr;
	}*/


}