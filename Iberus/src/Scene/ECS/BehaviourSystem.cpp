#include "Enginepch.h"
#include "BehaviourSystem.h"
#include "Scene.h"
#include "Behaviour.h"
#include "Components.h"
#include "Engine.h"
#include "ScriptHost.h"
#include "FileSystem.h"
#include "Log.h"

#include <unordered_map>

namespace Iberus {

		namespace {
		struct CSharpScriptState {
			ScriptHost::ScriptHandle handle;
			std::string assemblyPath;
			std::string typeName;
		};
		std::unordered_map<EntityId, CSharpScriptState> scriptHandles;
		static bool s_loggedScriptHostNotInit = false;
	}

	void BehaviourSystem::ClearScriptHandles() {
		s_loggedScriptHostNotInit = false;
		scriptHandles.clear();
	}

	void BehaviourSystem::Update(World& world, Scene& scene, double deltaTime) {
		for (auto& [type, list] : scene.GetRegisteredBehaviours()) {
			for (auto& [entityId, behaviour] : list) {
				if (!world.IsAlive(entityId)) {
					continue;
				}
				auto* active = world.GetComponent<ActiveComponent>(entityId);
				if (active && !active->Active) {
					continue;
				}
				behaviour->Update(entityId, world, deltaTime);
			}
		}

		auto* scriptHost = &Engine::Instance()->GetScriptHost();
		if (!scriptHost->IsInitialized()) {
			if (!s_loggedScriptHostNotInit) {
				s_loggedScriptHostNotInit = true;
				IB_CORE_WARN("[Scripts] ScriptHost not initialized. Check App/Iberus.Scripts.runtimeconfig.json and App/Iberus.Scripts.dll.");
			}
			return;
		}

		auto* scriptStorage = world.GetStorage<ScriptComponent>();
		if (!scriptStorage) {
			return;
		}

		std::string baseDir = Engine::Instance()->GetScriptBaseDir();
		if (baseDir.empty()) {
			baseDir = FileSystem::GetExeDirectory();
		}

		// Normalize assembly path: legacy "Demo.Scripts.dll" -> "Assets/Scripts/Demo.Scripts.dll"
		auto resolveAssemblyPath = [](const std::string& p) -> std::string {
			if (p.find('/') != std::string::npos || p.find('\\') != std::string::npos) {
				return p;
			}
			return "Assets/Scripts/" + p;
		};

		for (auto [entityId, comp] : *scriptStorage) {
			if (!world.IsAlive(entityId)) {
				continue;
			}
			auto* active = world.GetComponent<ActiveComponent>(entityId);
			if (active && !active->Active) {
				continue;
			}
			if (comp.AssemblyPath.empty() || comp.TypeName.empty()) {
				continue;
			}

			auto it = scriptHandles.find(entityId);
			bool needReload = (it == scriptHandles.end()) ||
				(it->second.assemblyPath != comp.AssemblyPath || it->second.typeName != comp.TypeName);

			if (needReload) {
				if (it != scriptHandles.end()) {
					scriptHost->UnloadScript(it->second.handle);
					scriptHandles.erase(it);
					it = scriptHandles.end();
				}
				std::string resolvedPath = resolveAssemblyPath(comp.AssemblyPath);
			ScriptHost::ScriptHandle handle = scriptHost->LoadScript(resolvedPath, comp.TypeName, baseDir);
				if (!handle) {
					IB_CORE_WARN("[Scripts] LoadScript failed: {} / {} (baseDir={})", comp.AssemblyPath, comp.TypeName, baseDir);
					continue;
				}
				CSharpScriptState state;
				state.handle = handle;
				state.assemblyPath = comp.AssemblyPath;
				state.typeName = comp.TypeName;
				scriptHost->CallInit(handle, static_cast<uint64_t>(entityId), &world);
				scriptHandles[entityId] = std::move(state);
				scriptHost->CallUpdate(handle, static_cast<uint64_t>(entityId), &world, deltaTime);
			} else {
				scriptHost->CallUpdate(it->second.handle, static_cast<uint64_t>(entityId), &world, deltaTime);
			}
		}

		for (auto it = scriptHandles.begin(); it != scriptHandles.end(); ) {
			if (!world.IsAlive(it->first)) {
				scriptHost->UnloadScript(it->second.handle);
				it = scriptHandles.erase(it);
			} else {
				++it;
			}
		}
	}

}
