#pragma once

#include "../../common/com_defines.h"
#include "../app.h"
#include "../user.h"
#include "../commands.h"
#include "../stream.h"
#include "../packet.h"

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
void    SV_WriteDebugString(ZStringHeader* str);
u8      SV_ParseCommandString(char* str, char** tokens, i32 numTokens);
