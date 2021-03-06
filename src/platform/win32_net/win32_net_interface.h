#pragma once

#include "../../common/common.h"

///////////////////////////////////////
// Functions
///////////////////////////////////////

i32 Net_Init();
i32 Net_OpenSocket(u16 port, u16* portOpened);
i32 Net_CloseSocket(i32 socketIndex);
i32 Net_Shutdown();

i32 Net_SendTo(i32 transmittingSocketIndex, ZNetAddress* address, u16 port, char* data, i32 dataSize);
i32 Net_Read(i32 socketIndex, ZNetAddress* sender,  MemoryBlock* dataPtr);

void Net_RunLoopbackTest();
