#pragma once

#include "../common/com_module.h"

#include "sim_types.h"

void Sim_AddEntBlock(SimScene* scene, SimEntBlock block);
i32 Sim_AddEntity(SimScene* scene, f32 x, f32 y, f32 z);
void Sim_InitScene(
    SimScene* scene, ByteBuffer cmdBufferA, ByteBuffer cmdBufferB);
i32 Sim_Tick(SimScene* scene, f32 deltaTime);
