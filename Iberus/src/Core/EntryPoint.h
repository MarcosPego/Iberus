#pragma once

#ifdef IB_PLATFORM_WINDOWS

extern Iberus::Application* Iberus::CreateApplication();


int main(int argc, char** argv) {
	Iberus::Log::Init();
	Iberus::Log::GetCoreLogger()->warn("Initialized Log!");
	Iberus::Log::GetClientLogger()->info("Hello!");

	auto app = Iberus::CreateApplication();
	app->Boot();
	app->Run();
	delete app;
}
#endif // IB_PLATFORM_WINDOWS