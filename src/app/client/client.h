#pragma once

#include "../user.h"
#include "../../common/com_defines.h"
#include "../app.h"

void    CL_Init();
i32     CL_IsRunning();
void    CL_Tick(f32 deltaTime);
void    CL_PopulateRenderScene(RenderScene* scene, i32 maxObjects, i32 texIndex, f32 interpolateTime);
