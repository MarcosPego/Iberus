#pragma once

#ifdef IB_PLATFORM_WINDOWS
	#ifdef IB_BUILD_DLL
		#define IBERUS_API __declspec(dllexport)
	#else
		#define IBERUS_API __declspec(dllimport)
	#endif // IB_BUILD_DLL
#endif // IB_PLATFORM_WINDOWS

#define BIT(x)  (1 << x)
