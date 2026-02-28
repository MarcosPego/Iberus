#pragma once

#include "Core.h"
#include "FileSystem.h"
#include "IProvider.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "Mesh.h"
#include "Material.h"
#include "MaterialSerializer.h"
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

			// Extract basename for path fallbacks (e.g. "assets/shaders/baseRaymarchingShader" -> "baseRaymarchingShader")
			std::string basename = id;
			auto slash = id.find_last_of("/\\");
			if (slash != std::string::npos) {
				basename = id.substr(slash + 1);
			}

			std::string vertPaths[] = {
				id + ".vert",
				"Assets/Shaders/" + basename + ".vert",
				"assets/shaders/" + basename + ".vert",
				"Projects/Demo/Assets/Shaders/" + basename + ".vert",
			};
			std::string fragPaths[] = {
				id + ".frag",
				"Assets/Shaders/" + basename + ".frag",
				"assets/shaders/" + basename + ".frag",
				"Projects/Demo/Assets/Shaders/" + basename + ".frag",
			};

			Buffer vertexBuffer;
			Buffer fragBuffer;
			for (const auto& p : vertPaths) {
				vertexBuffer = provider->GetRawFileBuffer(p);
				if (!vertexBuffer.Invalid() && vertexBuffer.GetSize() > 0) {
					break;
				}
			}
			for (const auto& p : fragPaths) {
				fragBuffer = provider->GetRawFileBuffer(p);
				if (!fragBuffer.Invalid() && fragBuffer.GetSize() > 0) {
					break;
				}
			}

			if (vertexBuffer.Invalid() || vertexBuffer.GetSize() == 0) {
				Log::GetCoreLogger()->error("Shader vertex stage not found for id: {}", id);
				return nullptr;
			}
			if (fragBuffer.Invalid() || fragBuffer.GetSize() == 0) {
				Log::GetCoreLogger()->error("Shader fragment stage not found for id: {}", id);
				return nullptr;
			}

			auto shader = std::make_unique<Shader>(id, std::move(vertexBuffer), std::move(fragBuffer));
			auto* ptr = shader.get();
			resources.emplace(id, std::move(shader));
			return dynamic_cast<Shader*>(ptr);
		}

		template<>
		ComputeShader* CreateResource(const std::string& id, IProvider* provider) {
			if (!provider) {
				return nullptr;
			}

			std::string basename = id;
			auto slash = id.find_last_of("/\\");
			if (slash != std::string::npos) {
				basename = id.substr(slash + 1);
			}

			std::string compPaths[] = {
				id + ".comp",
				"Assets/Shaders/" + basename + ".comp",
				"assets/shaders/" + basename + ".comp",
				"Projects/Demo/Assets/Shaders/" + basename + ".comp",
			};

			Buffer compBuffer;
			for (const auto& p : compPaths) {
				compBuffer = provider->GetRawFileBuffer(p);
				if (!compBuffer.Invalid() && compBuffer.GetSize() > 0) {
					break;
				}
			}

			if (compBuffer.Invalid() || compBuffer.GetSize() == 0) {
				Log::GetCoreLogger()->warn("Compute shader not found for id: {}", id);
				return nullptr;
			}

			auto shader = std::make_unique<ComputeShader>(id, std::move(compBuffer));
			auto* ptr = shader.get();
			resources.emplace(id, std::move(shader));
			return dynamic_cast<ComputeShader*>(ptr);
		}

		template<>
		Material* CreateResource(const std::string& id, IProvider* provider) {
			if (!provider) {
				return nullptr;
			}

			std::string path = "Assets/Materials/" + id + ".mat";
			auto buffer = provider->GetRawFileBuffer(path);
			if (buffer.Invalid()) {
				return nullptr;
			}

			auto mat = MaterialSerializer::Deserialize(buffer, *this, provider);
			if (!mat) {
				return nullptr;
			}

			auto* ptr = mat.get();
			resources[id] = std::move(mat);
			return dynamic_cast<Material*>(ptr);
		}

		/// Register an already-created resource (e.g. material loaded from arbitrary path via MaterialSerializer).
		template<typename T>
		T* RegisterResource(const std::string& id, std::unique_ptr<T> resource) {
			if (!resource) {
				return nullptr;
			}
			T* ptr = resource.get();
			resources[id] = std::move(resource);
			return dynamic_cast<T*>(ptr);
		}

	private:

		template<typename T = Resource, typename... Args>
		T* InitializeResource(const std::string& id, Args&&... args) {
			auto resource = std::make_unique<T>(id, std::forward<Args>(args)...);
			auto* ptr = resource.get();
			resources[id] = std::move(resource);
			return dynamic_cast<T*>(ptr);
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

