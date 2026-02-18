#pragma once

#include "Core.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)  // stdext::checked_array_iterator deprecation (spdlog/fmt)
#pragma warning(disable : 4251)  // shared_ptr<spdlog::logger> needs dll-interface (spdlog type)
#endif
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace Iberus {
	class IBERUS_API Log {
	public:
		Log(const Log&) = delete;
		Log& operator=(const Log&) = delete;

		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return clientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> coreLogger;
		static std::shared_ptr<spdlog::logger> clientLogger;
	};
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
