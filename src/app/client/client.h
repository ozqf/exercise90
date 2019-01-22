#pragma once

#include "../user.h"
#include "../../common/com_defines.h"
#include "../app.h"
#include "../commands.h"
#include "../stream.h"

void    CL_Init();
void    CL_Shutdown();
i32     CL_IsRunning();
void    CL_Tick(ByteBuffer* sysEvents, f32 deltaTime);
void    CL_PopulateRenderScene(RenderScene* scene, i32 maxObjects, i32 texIndex, f32 interpolateTime);
void    CL_SetLocalUser(UserIds ids);
