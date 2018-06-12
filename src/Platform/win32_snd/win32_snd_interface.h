#pragma once

#include "../../common/com_module.h"

struct SoundInterface
{
    u8 (*Snd_Init)();
    u8 (*Snd_Shutdown)();
    u8 (*Snd_LoadSound)(u8* data, i32 numBytes);
};

typedef SoundInterface (Func_LinkToSound)();
