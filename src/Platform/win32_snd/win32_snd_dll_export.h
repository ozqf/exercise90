#pragma once

#include "../../common/com_defines.h"
#include "../win32_system_include.h"

extern "C"
SoundInterface __declspec(dllexport) LinkToSound()
{
    SoundInterface snd;
    snd.Snd_Init = Snd_Init;
    snd.Snd_Shutdown = Snd_Shutdown;

    return snd;
}
