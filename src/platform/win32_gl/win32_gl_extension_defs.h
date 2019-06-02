#pragma once

#include "win32_gl_module.cpp"

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
