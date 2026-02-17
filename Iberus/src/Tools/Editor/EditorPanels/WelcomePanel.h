#pragma once

#include "IGUIContext.h"

#include <string>
#include <vector>

namespace Iberus {

	class Application;

	class WelcomePanel {
	public:
		explicit WelcomePanel(Application& app);

		void OnDraw(IGUIContext& gui);

	private:
		void RefreshDiscoveredProjects();
		bool TryCreateProject(const std::string& name);

		Application& application;
		static constexpr size_t kPathBufferSize = 512;
		static constexpr size_t kNameBufferSize = 64;
		char openProjectPath[kPathBufferSize]{};
		char createProjectName[kNameBufferSize]{};
		std::vector<std::string> discoveredProjectPaths;
		int selectedProjectIndex{ -1 };
	};

}
