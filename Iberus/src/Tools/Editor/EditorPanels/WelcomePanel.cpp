#include "Enginepch.h"
#include "WelcomePanel.h"

namespace Iberus {

	void WelcomePanel::OnDraw(IGUIContext& gui) {
		if (gui.BeginWindow("Hello")) {
			gui.Text("Hello World");
			gui.EndWindow();
		}
	}

}
