// Minimal coreclr_delegates.h for .NET hosting
// Based on dotnet/runtime src/native/corehost/coreclr_delegates.h

#ifndef __CORECLR_DELEGATES_H__
#define __CORECLR_DELEGATES_H__

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
typedef wchar_t char_t;
#else
typedef char char_t;
#endif

#ifdef _WIN32
#define CORECLR_DELEGATE_CALLTYPE __stdcall
#else
#define CORECLR_DELEGATE_CALLTYPE
#endif

// Special delegate type for [UnmanagedCallersOnly] methods
#ifdef _WIN32
#define UNMANAGEDCALLERSONLY_METHOD L"NativeAOT"
#else
#define UNMANAGEDCALLERSONLY_METHOD "NativeAOT"
#endif

enum hostfxr_delegate_type {
    hdt_load_assembly_and_get_function_pointer = 0,
    hdt_get_function_pointer = 1
};

typedef int (*load_assembly_and_get_function_pointer_fn)(
    const char_t* assembly_path,
    const char_t* type_name,
    const char_t* method_name,
    const char_t* delegate_type_name,
    void* reserved,
    void** delegate);

#endif // __CORECLR_DELEGATES_H__
