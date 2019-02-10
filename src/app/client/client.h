#pragma once

#include "../user.h"
#include "../../common/com_defines.h"
#include "../app.h"
#include "../commands.h"
#include "../stream.h"

void    CL_Init(ZNetAddress serverAddress);
void    CL_Shutdown();
i32     CL_IsRunning();
void    CL_Tick(ByteBuffer* sysEvents, f32 deltaTime, u32 platformFrame);
void    CL_PopulateRenderScene(RenderScene* scene, i32 maxObjects, i32 texIndex, f32 interpolateTime);
void    CL_SetLocalUser(UserIds ids);
void    CL_WriteDebugString(ZStringHeader* str);
u8      CL_ParseCommandString(char* str, char** tokens, i32 numTokens);
