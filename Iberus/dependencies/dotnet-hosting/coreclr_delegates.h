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

// Special delegate type for [UnmanagedCallersOnly] methods - must be ((const char_t*)-1) per dotnet/runtime
#define UNMANAGEDCALLERSONLY_METHOD ((const char_t*)-1)

// Must match dotnet/runtime src/native/corehost/hostfxr.h - wrong values request COM activation!
enum hostfxr_delegate_type {
    hdt_com_activation = 0,
    hdt_load_in_memory_assembly = 1,
    hdt_winrt_activation = 2,
    hdt_com_register = 3,
    hdt_com_unregister = 4,
    hdt_load_assembly_and_get_function_pointer = 5,
    hdt_get_function_pointer = 6,
    hdt_load_assembly = 7,
    hdt_load_assembly_bytes = 8,
};

typedef int (*load_assembly_and_get_function_pointer_fn)(
    const char_t* assembly_path,
    const char_t* type_name,
    const char_t* method_name,
    const char_t* delegate_type_name,
    void* reserved,
    void** delegate);

#endif // __CORECLR_DELEGATES_H__
