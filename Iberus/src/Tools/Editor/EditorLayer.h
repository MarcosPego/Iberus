#pragma once

#include "Layer.h"
#include "Editor.h"

namespace Iberus {

	class Application;

	class IBERUS_API EditorLayer : public Layer {
	public:
		EditorLayer() : Layer("Editor") {}

		void OnUpdate(double deltaTime) override;

	private:
		Editor editor;
	};

}
