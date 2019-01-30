#pragma once

#include "../../common/com_defines.h"
#include "../app.h"
#include "../user.h"
#include "../commands.h"
#include "../stream.h"
#include "../packet.h"

void    SV_Init();
void    SV_Shutdown();
UserIds SV_CreateLocalUser();
i32     SV_IsRunning();
void    SV_Tick(ByteBuffer* platformCommands, f32 deltaTime);
void    SV_PopulateRenderScene(
            RenderScene* scene, i32 maxObjects, i32 texIndex, f32 interpolateTime);
//UserIds SV_CreateLocalUser();
void    SV_WriteDebugString(ZStringHeader* str);
