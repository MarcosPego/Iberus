#pragma once

#include "Core.h"
#include "Behaviour.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace Iberus {

	class IBERUS_API BehaviourRegistry {
	public:
		using FactoryFn = std::function<std::unique_ptr<Behaviour>()>;

		/// Register a behaviour type. Overwrites existing.
		static void Register(const std::string& typeName, FactoryFn factory);

		/// Create a behaviour instance by type name. Returns nullptr if unknown.
		static std::unique_ptr<Behaviour> Create(const std::string& typeName);
	};

}
