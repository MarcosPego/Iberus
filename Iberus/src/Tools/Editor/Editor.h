#pragma once

#include "IGUIContext.h"
#include "WelcomePanel.h"
#include <memory>
#include <vector>

namespace Iberus {

	class IBERUS_API Editor {
	public:
		Editor();
		~Editor();

		void OnUpdate(double deltaTime, IGUIContext* gui);

	private:
		std::unique_ptr<WelcomePanel> welcomePanel;
	};

}
