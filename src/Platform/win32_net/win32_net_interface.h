#pragma once

#include "../../common/com_module.h"

///////////////////////////////////////
// Types
///////////////////////////////////////

///////////////////////////////////////
// Functions
///////////////////////////////////////

i32 Net_Init();
i32 Net_OpenSocket(u16 port);
i32 Net_Shutdown();

i32 Net_SendTo(i32 transmittingSocketIndex, char* address, u16 port, char* data, i32 dataSize);
i32 Net_Read();

void WNet_RunLoopbackTest();
