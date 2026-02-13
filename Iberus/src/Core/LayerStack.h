#pragma once

#include <functional>

namespace Iberus {

	class Layer;

	class IBERUS_API LayerStack {
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void PopOverlay(Layer* layer);
		void PopLayer(Layer* layer);

		void ForEachLayer(const std::function<void(Layer*)>& fn) const;

	private:
		std::vector<Layer*> layers;
		std::vector<Layer*> overlays;
	};
}


