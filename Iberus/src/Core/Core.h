#pragma once

#ifdef IB_PLATFORM_WINDOWS
#if IB_DYNAMIC_LINK
	#ifdef IB_BUILD_DLL
		#define IBERUS_API __declspec(dllexport)
	#else
		#define IBERUS_API __declspec(dllimport)
	#endif // IB_BUILD_DLL
#else
	#define IBERUS_API
#endif

#endif // IB_PLATFORM_WINDOWS

#define BIT(x)  (1 << x)
