#pragma once

#include "../common/com_module.h"

#include "sim_types.h"
#include "sim_command_types.h"

void    Sim_AddEntBlock(SimScene* scene, SimEntBlock block);
i32     Sim_AddEntity(SimScene* scene, SimEntityDef* def);
void    Sim_InitScene(
            SimScene* scene, 
            ByteBuffer cmdBufferA,
            ByteBuffer cmdBufferB,
            SimEntity* entityMemory,
            i32 maxEntities);
i32     Sim_Tick(SimScene* scene, f32 deltaTime);
i32     Sim_Execute(SimScene* scene, SimCmd* header);
i32     Sim_CalcEntityArrayBytes(i32 capacity);
