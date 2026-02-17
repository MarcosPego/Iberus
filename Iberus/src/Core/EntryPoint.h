#pragma once

#ifdef IB_PLATFORM_WINDOWS

#include "FileSystem.h"
#include <filesystem>

extern Iberus::Application* Iberus::CreateApplication();


int main(int argc, char** argv) {
	Iberus::Log::Init();
	Iberus::Log::GetCoreLogger()->warn("Initialized Log!");
	Iberus::Log::GetClientLogger()->info("Hello!");

	std::string exeDir = Iberus::FileSystem::GetExeDirectory();
	if (!exeDir.empty()) {
		std::filesystem::path defaultProject = std::filesystem::path(exeDir) / "Projects" / "Demo";
		if (std::filesystem::exists(defaultProject) && std::filesystem::is_directory(defaultProject)) {
			std::filesystem::current_path(defaultProject);
		} else {
			std::filesystem::current_path(exeDir);
		}
	}

	auto app = Iberus::CreateApplication();
	app->Boot();
	app->Run();
	delete app;
}
#endif // IB_PLATFORM_WINDOWS