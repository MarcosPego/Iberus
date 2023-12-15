#include "Enginepch.h"
#include "LayerStack.h"

namespace Iberus {
	LayerStack::LayerStack() {

	}

	LayerStack::~LayerStack() {
		for (Layer* layer : layers) {
			delete layer;
		}

		for (Layer* overlay : overlays) {
			delete overlay;
		}
	}

	void LayerStack::PushLayer(Layer* layer) {
		layers.emplace_back(layer);
	}

	void LayerStack::PushOverlay(Layer* layer) {
		overlays.emplace_back(layer);
	}

	void LayerStack::PopOverlay(Layer* layer) {
		/*auto it = std::find(overlays.begin(), overlays.end(), overlays);
		if (it != overlays.end()) {
			overlays.erase(it);
		}*/
	}

	void LayerStack::PopLayer(Layer* layer) {
		/*auto it = std::find(layers.begin(), layers.end(), layers);
		if (it != layers.end()) {
			layers.erase(it);
		}*/
	}
}