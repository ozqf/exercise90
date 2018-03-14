#pragma once

#include "win32_system_include.h"

#include "win32_main.h"


f32 Win32_InitFloatTime()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    global_secondsPerTick = 1.0 / (double)frequency.QuadPart;
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    global_timeCount = counter.QuadPart;
    return 0;
}

f32 Win32_FloatTime()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    __int64 interval = counter.QuadPart - global_timeCount;
    global_timeCount = counter.QuadPart;

    double secondsGoneBy = (double)interval * global_secondsPerTick;
    global_timePassed += secondsGoneBy;
    return (float)global_timePassed;
}
