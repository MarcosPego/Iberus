#pragma once

#include "Core.h"
#include <string>

namespace Iberus {

	class Event;

	class IBERUS_API Layer {
	public:
		Layer(const std::string& name = "Layer") : debugName(name) {}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(double deltaTime) {}
		virtual void OnEvent(Event& event) {}

		const std::string& GetName() const { return debugName; }

	protected:
		std::string debugName;
	};

}
