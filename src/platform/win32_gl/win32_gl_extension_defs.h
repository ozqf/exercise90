#pragma once

#include "win32_gl_module.cpp"

// v-sync
#define PROCNAME_SWAP_INTERVAL "wglSwapIntervalEXT"
typedef BOOL WINAPI wgl_swap_interval_ext(int interval);


struct Extensions
{
    wgl_swap_interval_ext* SwapInterval;
};
