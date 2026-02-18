#pragma once

#include "Core.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <cstdint>

namespace Iberus {

	class World;

	/// Hosts the .NET runtime and manages C# script instances.
	/// Loads Iberus.Scripts.dll and dispatches Init/Update calls.
	class IBERUS_API ScriptHost {
	public:
		/// Opaque handle to a C# script instance.
		using ScriptHandle = void*;

		ScriptHost();
		~ScriptHost();

		ScriptHost(const ScriptHost&) = delete;
		ScriptHost& operator=(const ScriptHost&) = delete;

		/// Initialize the runtime using the given runtime config path.
		/// Typical path: "{exeDir}/Iberus.Scripts.runtimeconfig.json"
		bool Initialize(const std::string& runtimeConfigPath);

		/// Load a script type and create an instance. Returns null on failure.
		ScriptHandle LoadScript(const std::string& assemblyPath, const std::string& typeName,
			const std::string& baseDir);

		/// Call Init on the script instance.
		void CallInit(ScriptHandle handle, uint64_t entityId, World* world);

		/// Call Update on the script instance.
		void CallUpdate(ScriptHandle handle, uint64_t entityId, World* world, double deltaTime);

		/// Release a script instance. Call when entity is destroyed or script unloaded.
		void UnloadScript(ScriptHandle handle);

		/// Unload all script instances. Call on Play/Stop for hot-reload.
		void UnloadAll();

		/// Shutdown the runtime. Call on engine shutdown.
		void Shutdown();

		bool IsInitialized() const { return initialized; }

	private:
		struct Impl;
		std::unique_ptr<Impl> impl;
		bool initialized{ false };
	};

}
