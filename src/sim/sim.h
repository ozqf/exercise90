#pragma once

#include "../common/com_module.h"

#include "sim_types.h"

void Sim_AddEntBlock(SimScene* scene, SimEntBlock block);
SimEntId Sim_AddEntity(SimScene* scene, f32 x, f32 y, f32 z);
void Sim_InitScene(
    SimScene* scene, ByteBuffer cmdBufferA, ByteBuffer cmdBufferB);
