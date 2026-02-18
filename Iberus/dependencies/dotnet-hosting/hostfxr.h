// Minimal hostfxr.h for .NET hosting
// Based on dotnet/runtime src/native/corehost/hostfxr

#ifndef __HOSTFXR_H__
#define __HOSTFXR_H__

#include "coreclr_delegates.h"
#include <stddef.h>
#include <stdint.h>

typedef void* hostfxr_handle;

typedef int (*hostfxr_initialize_for_runtime_config_fn)(
    const char_t* config_path,
    const struct hostfxr_initialize_parameters* parameters,
    hostfxr_handle* host_context_handle);

typedef int (*hostfxr_get_runtime_delegate_fn)(
    hostfxr_handle host_context_handle,
    int type,
    void** delegate);

typedef int (*hostfxr_close_fn)(hostfxr_handle host_context_handle);

struct hostfxr_initialize_parameters {
    size_t size;
    const char_t* host_path;
    const char_t* dotnet_root;
};

#endif // __HOSTFXR_H__
