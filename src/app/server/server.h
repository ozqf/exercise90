#pragma once

#include "../../common/com_defines.h"

void SV_Init();
i32 SV_IsRunning();
void SV_Tick(f32 deltaTime);
void SV_PopulateRenderScene(RenderScene* scene, i32 maxObjects, i32 texIndex, f32 interpolateTime);
