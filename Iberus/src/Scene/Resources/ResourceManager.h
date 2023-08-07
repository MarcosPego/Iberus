#pragma once

#include "FileSystem.h"
#include "IProvider.h"
#include "Shader.h"
#include "Mesh.h"
#include "Resource.h"

namespace Iberus {

	class ResourceManager {
	public:

		template<typename T = Resource>
		T* GetResource(const std::string& id) {
			if (resources.find(id)) {
				return dynamic_cast<T*>(resources.at(id));			
			}

			return nullptr;
		}

		template<typename T = Resource>
		T* GetOrCreateResource(const std::string& id, IProvider* provider = nullptr) {
			auto* resource = GetResource<T>(id);
			return (resource) ? resource : CreateResource<T>(id, provider);
		}

		template<typename T = Resource>
		T* CreateResource(const std::string& id, IProvider* provider) {
			/// Load From disk
			return InitializeResource<T>(const std::string & id)
		}


		Shader* CreateResource(const std::string& id, IProvider* provider) {
			if (!provider) {
				return nullptr;
			}

			auto vertexBuffer = provider->GetRawFileBuffer(id + ".vertex");
			auto fragBuffer = provider->GetRawFileBuffer(id + ".frag");

			auto* shader = new Shader(id, std::move(vertexBuffer), std::move(fragBuffer));

			/// Load From disk
			return shader;
		}

	private:

		template<typename T = Resource>
		T* InitializeResource(const std::string& id, Buffer inboundBuffer);

		template<typename T = Resource>
		T* InitializeResource(const std::string& id);

	private:
		std::unordered_map<std::string, std::unique_ptr<Resource>> resources;
	};
}

