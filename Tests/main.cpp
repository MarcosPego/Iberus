#ifdef IB_PLATFORM_WINDOWS

#include "Log.h"
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <cstdlib>

// Forward declare test functions (implemented in test files)
int RunMathsTests();
int RunJsonValueTests();

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	// Add App/ to DLL search path so Iberus.dll is found
	{
		char path[MAX_PATH];
		if (GetModuleFileNameA(NULL, path, MAX_PATH) > 0) {
			std::filesystem::path p(path);
			std::string appDir = (p.parent_path() / "App").string();
			SetDllDirectoryA(appDir.c_str());
		}
	}

	// Init log (required by some Iberus code paths)
	Iberus::Log::Init();

	int failures = 0;

	failures += RunMathsTests();
	failures += RunJsonValueTests();

	if (failures > 0) {
		std::cerr << "\n" << failures << " test(s) failed.\n";
		return failures;
	}
	std::cout << "\nAll tests passed.\n";
	return 0;
}

#endif
