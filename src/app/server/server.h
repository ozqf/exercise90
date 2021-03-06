#pragma once

#include "../../common/com_defines.h"
#include "../user.h"

void    SV_Init();
void    SV_Shutdown();
UserIds SVU_CreateLocalUser();
i32     SV_IsRunning();
void    SV_Tick(ByteBuffer* platformCommands, f32 deltaTime);
void    SV_PopulateRenderScene(
            RenderScene* scene,
            i32 maxObjects,
            i32 texIndex,
            f32 interpolateTime,
            i32 drawScene,
            i32 drawTests);
void    SV_WriteDebugString(CharBuffer* str);
u8      SV_ParseCommandString(char* str, char** tokens, i32 numTokens);
