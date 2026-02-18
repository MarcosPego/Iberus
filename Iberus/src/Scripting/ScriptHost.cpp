#include "Enginepch.h"
#include "ScriptHost.h"
#include "World.h"
#include "FileSystem.h"

#include "nethost.h"
#include "hostfxr.h"
#include "coreclr_delegates.h"

#ifdef IB_PLATFORM_WINDOWS
#include <Windows.h>
#define LIB_HANDLE HMODULE
#define LOAD_LIB(path) LoadLibraryW((path).c_str())
#define GET_EXPORT(h, name) GetProcAddress((HMODULE)(h), name)
#define FREE_LIB(h) FreeLibrary((HMODULE)(h))
#endif

#include <filesystem>
#include <unordered_set>

namespace Iberus {

	static std::wstring ToWide(const std::string& utf8) {
#ifdef IB_PLATFORM_WINDOWS
		if (utf8.empty()) {
			return std::wstring();
		}
		int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), nullptr, 0);
		if (wlen <= 0) {
			return std::wstring();
		}
		std::wstring result(wlen, 0);
		MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), &result[0], wlen);
		return result;
#else
		(void)utf8;
		return std::wstring();
#endif
	}

	struct ScriptHost::Impl {
		LIB_HANDLE nethostLib{ nullptr };
		LIB_HANDLE hostfxrLib{ nullptr };

		hostfxr_initialize_for_runtime_config_fn initForConfig{ nullptr };
		hostfxr_get_runtime_delegate_fn getDelegate{ nullptr };
		hostfxr_close_fn close{ nullptr };

		load_assembly_and_get_function_pointer_fn loadAssembly{ nullptr };
		hostfxr_handle hostContext{ nullptr };

		using CreateInstanceFn = void* (*)(const char* assemblyPath, const char* typeName);
		using InitFn = void (*)(void* handle, uint64_t entityId, void* worldPtr);
		using UpdateFn = void (*)(void* handle, uint64_t entityId, void* worldPtr, double deltaTime);
		using DestroyInstanceFn = void (*)(void* handle);

		CreateInstanceFn createInstance{ nullptr };
		InitFn scriptInit{ nullptr };
		UpdateFn scriptUpdate{ nullptr };
		DestroyInstanceFn destroyInstance{ nullptr };

		std::string baseDir;
		std::unordered_set<void*> activeHandles;
	};

	ScriptHost::ScriptHost() : impl(std::make_unique<Impl>()) {
	}

	ScriptHost::~ScriptHost() {
		Shutdown();
	}

	bool ScriptHost::Initialize(const std::string& runtimeConfigPath) {
		if (initialized) {
			return true;
		}
#ifdef IB_PLATFORM_WINDOWS
		std::string exeDir = FileSystem::GetExeDirectory();
		std::wstring nethostPathW = ToWide(exeDir) + L"\\nethost.dll";
		impl->nethostLib = LOAD_LIB(nethostPathW);
		if (!impl->nethostLib) {
			IB_ERROR("ScriptHost: Failed to load nethost.dll from {}", exeDir);
			return false;
		}
		auto getHostfxrPathFn = (int (*)(char_t*, size_t*, const get_hostfxr_parameters*))GET_EXPORT(impl->nethostLib, "get_hostfxr_path");
		if (!getHostfxrPathFn) {
			IB_ERROR("ScriptHost: get_hostfxr_path not found in nethost.dll");
			FREE_LIB(impl->nethostLib);
			impl->nethostLib = nullptr;
			return false;
		}
		std::string assemblyPath = (std::filesystem::path(exeDir) / "Game.Scripts.dll").string();
		if (!std::filesystem::exists(assemblyPath)) {
			IB_ERROR("ScriptHost: Game.Scripts.dll not found at {}", assemblyPath);
			FREE_LIB(impl->nethostLib);
			impl->nethostLib = nullptr;
			return false;
		}
		wchar_t hostfxrPath[1024];
		size_t pathSize = 1024;
		get_hostfxr_parameters params{};
		params.size = sizeof(params);
		std::wstring assemblyPathW = ToWide(assemblyPath);
		params.assembly_path = assemblyPathW.c_str();
		params.dotnet_root = nullptr;
		int rc = getHostfxrPathFn(hostfxrPath, &pathSize, &params);
		if (rc != 0) {
			IB_ERROR("ScriptHost: get_hostfxr_path failed: {:x}", static_cast<unsigned>(rc));
			FREE_LIB(impl->nethostLib);
			impl->nethostLib = nullptr;
			return false;
		}
		impl->hostfxrLib = LoadLibraryW(hostfxrPath);
		if (!impl->hostfxrLib) {
			IB_ERROR("ScriptHost: Failed to load hostfxr");
			FREE_LIB(impl->nethostLib);
			impl->nethostLib = nullptr;
			return false;
		}
		impl->initForConfig = (hostfxr_initialize_for_runtime_config_fn)GET_EXPORT(impl->hostfxrLib, "hostfxr_initialize_for_runtime_config");
		impl->getDelegate = (hostfxr_get_runtime_delegate_fn)GET_EXPORT(impl->hostfxrLib, "hostfxr_get_runtime_delegate");
		impl->close = (hostfxr_close_fn)GET_EXPORT(impl->hostfxrLib, "hostfxr_close");
		if (!impl->initForConfig || !impl->getDelegate || !impl->close) {
			IB_ERROR("ScriptHost: hostfxr exports not found");
			Shutdown();
			return false;
		}
		std::wstring configPathW = ToWide(runtimeConfigPath);
		hostfxr_initialize_parameters initParams{};
		initParams.size = sizeof(initParams);
		initParams.host_path = nullptr;
		initParams.dotnet_root = nullptr;
		rc = impl->initForConfig(configPathW.c_str(), &initParams, &impl->hostContext);
		if (rc != 0 || !impl->hostContext) {
			IB_ERROR("ScriptHost: hostfxr_initialize_for_runtime_config failed: {:x}", static_cast<unsigned>(rc));
			Shutdown();
			return false;
		}
		void* loadAssemblyPtr = nullptr;
		rc = impl->getDelegate(impl->hostContext, hdt_load_assembly_and_get_function_pointer, &loadAssemblyPtr);
		if (rc != 0 || !loadAssemblyPtr) {
			IB_ERROR("ScriptHost: get_delegate failed: {:x}", static_cast<unsigned>(rc));
			Shutdown();
			return false;
		}
		impl->loadAssembly = (load_assembly_and_get_function_pointer_fn)loadAssemblyPtr;
		impl->baseDir = std::filesystem::path(runtimeConfigPath).parent_path().string();
		initialized = true;
		return true;
#else
		(void)runtimeConfigPath;
		IB_ERROR("ScriptHost: .NET hosting only supported on Windows");
		return false;
#endif
	}

	ScriptHost::ScriptHandle ScriptHost::LoadScript(const std::string& assemblyPath, const std::string& typeName,
		const std::string& baseDir) {
		if (!initialized || !impl->loadAssembly) {
			return nullptr;
		}
		std::string fullAssemblyPath = assemblyPath;
		if (!baseDir.empty() && !std::filesystem::path(assemblyPath).is_absolute()) {
			fullAssemblyPath = (std::filesystem::path(baseDir) / assemblyPath).string();
		}
		if (!std::filesystem::exists(fullAssemblyPath)) {
			IB_ERROR("ScriptHost: Assembly not found: {}", fullAssemblyPath);
			return nullptr;
		}
		// Bridge always comes from Game.Scripts (runtime) in exe dir
		if (impl->createInstance == nullptr) {
			std::string exeDir = FileSystem::GetExeDirectory();
			std::string runtimePath = (std::filesystem::path(exeDir) / "Game.Scripts.dll").string();
			if (!std::filesystem::exists(runtimePath)) {
				IB_ERROR("ScriptHost: Game.Scripts.dll not found at {}", runtimePath);
				return nullptr;
			}
			std::wstring runtimePathW = ToWide(runtimePath);
			const wchar_t* bridgeTypeW = L"GameScripts.ScriptBridge, Game.Scripts";
			void* createPtr = nullptr;
			int rc = impl->loadAssembly(runtimePathW.c_str(), bridgeTypeW, L"CreateInstance",
				UNMANAGEDCALLERSONLY_METHOD, nullptr, &createPtr);
			if (rc != 0 || !createPtr) {
				IB_ERROR("ScriptHost: Failed to get CreateInstance from Game.Scripts: {:x}", static_cast<unsigned>(rc));
				return nullptr;
			}
			impl->createInstance = (Impl::CreateInstanceFn)createPtr;
			void* initPtr = nullptr;
			rc = impl->loadAssembly(runtimePathW.c_str(), bridgeTypeW, L"Init",
				UNMANAGEDCALLERSONLY_METHOD, nullptr, &initPtr);
			if (rc != 0 || !initPtr) {
				return nullptr;
			}
			impl->scriptInit = (Impl::InitFn)initPtr;
			void* updatePtr = nullptr;
			rc = impl->loadAssembly(runtimePathW.c_str(), bridgeTypeW, L"Update",
				UNMANAGEDCALLERSONLY_METHOD, nullptr, &updatePtr);
			if (rc != 0 || !updatePtr) {
				return nullptr;
			}
			impl->scriptUpdate = (Impl::UpdateFn)updatePtr;
			void* destroyPtr = nullptr;
			rc = impl->loadAssembly(runtimePathW.c_str(), bridgeTypeW, L"DestroyInstance",
				UNMANAGEDCALLERSONLY_METHOD, nullptr, &destroyPtr);
			if (rc != 0 || !destroyPtr) {
				return nullptr;
			}
			impl->destroyInstance = (Impl::DestroyInstanceFn)destroyPtr;
		}
		void* handle = impl->createInstance(fullAssemblyPath.c_str(), typeName.c_str());
		if (handle) {
			impl->activeHandles.insert(handle);
		}
		return handle;
	}

	void ScriptHost::CallInit(ScriptHandle handle, uint64_t entityId, World* world) {
		if (handle && impl->scriptInit && world) {
			impl->scriptInit(handle, entityId, world);
		}
	}

	void ScriptHost::CallUpdate(ScriptHandle handle, uint64_t entityId, World* world, double deltaTime) {
		if (handle && impl->scriptUpdate && world) {
			impl->scriptUpdate(handle, entityId, world, deltaTime);
		}
	}

	void ScriptHost::UnloadScript(ScriptHandle handle) {
		if (handle && impl->destroyInstance) {
			impl->destroyInstance(handle);
			impl->activeHandles.erase(handle);
		}
	}

	void ScriptHost::UnloadAll() {
		if (impl->destroyInstance) {
			for (void* h : impl->activeHandles) {
				impl->destroyInstance(h);
			}
			impl->activeHandles.clear();
		}
	}

	void ScriptHost::Shutdown() {
		UnloadAll();
		if (impl->hostContext && impl->close) {
			impl->close(impl->hostContext);
			impl->hostContext = nullptr;
		}
		if (impl->hostfxrLib) {
			FREE_LIB(impl->hostfxrLib);
			impl->hostfxrLib = nullptr;
		}
		if (impl->nethostLib) {
			FREE_LIB(impl->nethostLib);
			impl->nethostLib = nullptr;
		}
		impl->initForConfig = nullptr;
		impl->getDelegate = nullptr;
		impl->close = nullptr;
		impl->loadAssembly = nullptr;
		impl->createInstance = nullptr;
		impl->scriptInit = nullptr;
		impl->scriptUpdate = nullptr;
		impl->destroyInstance = nullptr;
		initialized = false;
	}

}
