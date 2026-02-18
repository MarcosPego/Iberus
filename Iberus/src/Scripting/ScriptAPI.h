#pragma once

#include "Core.h"
#include <cstdint>
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef IB_PLATFORM_WINDOWS
#ifdef IB_BUILD_DLL
#define IB_SCRIPT_API __declspec(dllexport)
#else
#define IB_SCRIPT_API __declspec(dllimport)
#endif
#else
#define IB_SCRIPT_API
#endif

/// C API for P/Invoke from C# scripts.

typedef struct Iberus_Vec3 {
    float x;
    float y;
    float z;
} Iberus_Vec3;

/// World pointer is passed as void* from C# (IntPtr).
/// EntityId is uint64_t.

IB_SCRIPT_API int Iberus_World_GetComponent_Transform(void* worldPtr, uint64_t entityId,
    Iberus_Vec3* outPos, Iberus_Vec3* outRot, Iberus_Vec3* outScale);

IB_SCRIPT_API void Iberus_World_SetComponent_Transform(void* worldPtr, uint64_t entityId,
    const Iberus_Vec3* pos, const Iberus_Vec3* rot, const Iberus_Vec3* scale);

IB_SCRIPT_API int Iberus_World_IsAlive(void* worldPtr, uint64_t entityId);

IB_SCRIPT_API int Iberus_World_HasComponent_Transform(void* worldPtr, uint64_t entityId);

IB_SCRIPT_API uint64_t Iberus_World_CreateEntity(void* worldPtr);

IB_SCRIPT_API void Iberus_World_DestroyEntity(void* worldPtr, uint64_t entityId);

/// Input (KeyCode values: W=87, S=83, A=65, D=68, Q=81, E=69, etc.)
IB_SCRIPT_API int Iberus_Input_IsKeyPressed(int keyCode);

/// SDF component access
IB_SCRIPT_API int Iberus_World_HasComponent_SDF(void* worldPtr, uint64_t entityId);
IB_SCRIPT_API int Iberus_World_GetSDFPartCount(void* worldPtr, uint64_t entityId);
IB_SCRIPT_API int Iberus_World_GetSDFPartPosition(void* worldPtr, uint64_t entityId, int partIndex, Iberus_Vec3* outPos);
IB_SCRIPT_API void Iberus_World_SetSDFPartPosition(void* worldPtr, uint64_t entityId, int partIndex, const Iberus_Vec3* pos);

/// Debug: log message from C# scripts (shows in engine console/logger).
IB_SCRIPT_API void Iberus_Debug_Log(const char* message);

#ifdef __cplusplus
}
#endif
