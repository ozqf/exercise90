#pragma once

#include "../../common/com_defines.h"
#include "../win32_system_include.h"

extern "C"
SoundInterface __declspec(dllexport) LinkToSound()
{
    printf("SOUND: Built on %s at %s\n", __DATE__, __TIME__);
    SoundInterface snd;

    snd.Snd_Init = Snd_Init;
    snd.Snd_Shutdown = Snd_Shutdown;
    snd.Snd_ParseCommandString = Snd_ParseCommandString;

    snd.Snd_LoadSound = Snd_LoadSound;
    snd.Snd_Play2 = Snd_Play2;
    
    return snd;
}
