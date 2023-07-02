#include "Enginepch.h"
#include "Log.h"

namespace Iberus {
	std::shared_ptr<spdlog::logger> Log::coreLogger;
	std::shared_ptr<spdlog::logger> Log::clientLogger;

	void Log::Init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		coreLogger = spdlog::stdout_color_mt("IBERUS");
		coreLogger->set_level(spdlog::level::trace);
	
		clientLogger = spdlog::stdout_color_mt("APP");
		clientLogger->set_level(spdlog::level::trace);
	}
}

#define IB_CORE_TRACE(...)	::Iberus::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define IB_CORE_INFO(...)	::Iberus::Log::GetCoreLogger()->info(__VA_ARGS__);
#define IB_CORE_WARN(...)	::Iberus::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define IB_CORE_ERROR(...)	::Iberus::Log::GetCoreLogger()->error(__VA_ARGS__);
#define IB_CORE_FATAL(...)	::Iberus::Log::GetCoreLogger()->fatal(__VA_ARGS__);

#define IB_TRACE(...)		::Iberus::Log::GetClientLogger()->trace(__VA_ARGS__);
#define IB_INFO(...)		::Iberus::Log::GetClientLogger()->info(__VA_ARGS__);
#define IB_WARN(...)		::Iberus::Log::GetClientLogger()->warn(__VA_ARGS__);
#define IB_ERROR(...)		::Iberus::Log::GetClientLogger()->error(__VA_ARGS__);
#define IB_FATAL(...)		::Iberus::Log::GetClientLogger()->fatal(__VA_ARGS__);