#pragma once

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

	private:
		std::vector<Layer*> layers;
		std::vector<Layer*> overlays;
	};
}


