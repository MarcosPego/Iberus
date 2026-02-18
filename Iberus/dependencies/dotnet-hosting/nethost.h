// Minimal nethost.h for locating hostfxr
// Based on dotnet/runtime src/native/corehost/nethost/nethost.h

#ifndef HAVE_NETHOST_H
#define HAVE_NETHOST_H

#include <stddef.h>

#ifdef _WIN32
#define NETHOST_API __declspec(dllimport)
#define NETHOST_CALLTYPE __stdcall
typedef wchar_t char_t;
#else
#define NETHOST_API
#define NETHOST_CALLTYPE
typedef char char_t;
#endif

struct get_hostfxr_parameters {
    size_t size;
    const char_t* assembly_path;
    const char_t* dotnet_root;
};

#ifdef __cplusplus
extern "C" {
#endif

NETHOST_API int NETHOST_CALLTYPE get_hostfxr_path(
    char_t* buffer,
    size_t* buffer_size,
    const struct get_hostfxr_parameters* parameters);

#ifdef __cplusplus
}
#endif

#endif // HAVE_NETHOST_H
