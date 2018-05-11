#pragma once

#include "../../common/com_module.h"

struct SoundInterface
{
    u8 (*Snd_Init)();
    u8 (*Snd_Shutdown)();
};

typedef SoundInterface (Func_LinkToSound)();
