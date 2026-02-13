#include "Enginepch.h"
#include "Editor.h"
#include "WelcomePanel.h"

namespace Iberus {

	Editor::Editor() : welcomePanel(std::make_unique<WelcomePanel>()) {
	}

	Editor::~Editor() = default;

	void Editor::OnUpdate(double deltaTime, IGUIContext* gui) {
		if (gui) {
			welcomePanel->OnDraw(*gui);
		}
	}

}
