#pragma once

#include "Core.h"
#include "FileSystem.h"
#include "IProvider.h"
#include "Shader.h"
#include "Mesh.h"
#include "Resource.h"

namespace Iberus {

	class IBERUS_API ResourceManager {
	public:

		ResourceManager() = default;
		virtual ~ResourceManager() = default;

		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator= (const ResourceManager&) = delete;

		template<typename T = Resource>
		T* GetResource(const std::string& id) {
			if (resources.find(id) != resources.end()) {
				return dynamic_cast<T*>(resources.at(id).get());			
			}

			return nullptr;
		}

		template<typename T = Resource>
		T* GetOrCreateResource(const std::string& id, IProvider* provider = nullptr) {
			auto* resource = GetResource<T>(id);
			return (resource) ? resource : CreateResource<T>(id, provider);
		}

		template<typename T = Resource, typename... Args>
		T* CreateResource(const std::string& id, IProvider* provider, Args&&... args) {
			if (!provider) {
				return nullptr;
			}

			/// Load From Disk
			auto buffer = provider->GetRawFileBuffer(id);

			if (buffer.Invalid()) {
				Log::GetCoreLogger()->error("Invalid buffer from id: " + id);
				return nullptr;
			}

			return InitializeResource<T>(id, std::move(buffer), std::forward<Args>(args)...);
		}

		template<typename T = Resource, typename... Args>
		T* CreateResource(const std::string& id, Args&&... args) {
			return InitializeResource<T>(id, std::forward<Args>(args)...);
		}

		template<>
		Shader* CreateResource(const std::string& id, IProvider* provider) {
			if (!provider) {
				return nullptr;
			}

			auto vertexBuffer = provider->GetRawFileBuffer(id + ".vert");
			auto fragBuffer = provider->GetRawFileBuffer(id + ".frag");

			auto* shader = new Shader(id, std::move(vertexBuffer), std::move(fragBuffer));

			resources.emplace(id, shader);
			return dynamic_cast<Shader*>(resources.at(id).get());
		}

	private:

		template<typename T = Resource, typename... Args>
		T* InitializeResource(const std::string& id, Args&&... args) {
			auto* resource = new T(id, std::forward<Args>(args)...);
			resources.emplace(id, resource);

			return dynamic_cast<T*>(resources.at(id).get());
		}

		/*template<typename T = Resource, typename... Args>
		T* InitializeResource(const std::string& id) {
			auto* resource = new T(id);
			resources.emplace(id, resource);

			return dynamic_cast<T*>(resources.at(id).get());
		}*/

	private:
		std::unordered_map<std::string, std::unique_ptr<Resource>> resources;
	};
}

