#pragma once

#ifdef IB_PLATFORM_WINDOWS

#include "FileSystem.h"
#include <filesystem>
#include <cstring>
#include <Windows.h>

extern Iberus::Application* Iberus::CreateApplication(int argc, char** argv);


int main(int argc, char** argv) {
	// Add App/ to DLL search path so all runtime DLLs are found there
	{
		char path[MAX_PATH];
		if (GetModuleFileNameA(NULL, path, MAX_PATH) > 0) {
			std::filesystem::path p(path);
			std::string appDir = (p.parent_path() / "App").string();
			SetDllDirectoryA(appDir.c_str());
		}
	}

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

	auto app = Iberus::CreateApplication(argc, argv);
	app->Boot();
	app->Run();
	delete app;
}
#endif // IB_PLATFORM_WINDOWS