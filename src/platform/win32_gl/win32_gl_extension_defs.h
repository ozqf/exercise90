#pragma once

#include "win32_gl_module.cpp"

// Windows specific defines

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

    // Accepted as bits in the attribute value for WGL_CONTEXT_FLAGS in
    // <*attribList>:

#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

// Accepted as bits in the attribute value for
// WGL_CONTEXT_PROFILE_MASK_ARB in <*attribList>:
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

//New errors returned by GetLastError:
#define ERROR_INVALID_VERSION_ARB               0x2095
#define ERROR_INVALID_PROFILE_ARB               0x2096

// v-sync
#define PROCNAME_SWAP_INTERVAL "wglSwapIntervalEXT"
typedef BOOL WINAPI wgl_swap_interval_ext(int interval);

// Context Escalation
#define PROCNAME_CREATE_CONTEXT_ATTRIBS "wglCreateContextAttribsARB"
typedef HGLRC WINAPI wglCreateContextAttribsARB(
    HDC hDC, HGLRC hShareContext, const int* attribList);

/*
HGLRC wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int* attribList);
*/

struct Extensions
{
    wgl_swap_interval_ext* SwapInterval;
    wglCreateContextAttribsARB* CreateContextAttribs;
};
