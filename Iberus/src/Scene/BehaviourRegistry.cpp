#include "Enginepch.h"
#include "BehaviourRegistry.h"
#include "CameraController.h"
#include "SceneViewerCameraBehaviour.h"

#include <unordered_map>

namespace Iberus {

	namespace {
		std::unordered_map<std::string, BehaviourRegistry::FactoryFn>& GetFactories() {
			static std::unordered_map<std::string, BehaviourRegistry::FactoryFn> factories;
			return factories;
		}

		void RegisterBuiltInBehaviours() {
			auto& factories = GetFactories();
			if (!factories.empty()) {
				return;
			}
			factories["b_camera_controller"] = []() { return std::make_unique<CameraController>(); };
			factories["b_scene_viewer_camera"] = []() { return std::make_unique<SceneViewerCameraBehaviour>(); };
		}
	}

	void BehaviourRegistry::Register(const std::string& typeName, FactoryFn factory) {
		GetFactories()[typeName] = std::move(factory);
	}

	std::unique_ptr<Behaviour> BehaviourRegistry::Create(const std::string& typeName) {
		RegisterBuiltInBehaviours();
		auto& factories = GetFactories();
		auto it = factories.find(typeName);
		if (it == factories.end()) {
			return nullptr;
		}
		return it->second();
	}

}
