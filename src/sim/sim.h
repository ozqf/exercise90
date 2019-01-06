#pragma once

#include "../common/com_module.h"

#include "sim_types.h"

void Sim_GetEntityList(SimEntity** ptr, i32* max);
void Sim_InitScene(SimScene* scene);
void Sim_AddEntBlock(SimScene* scene, SimEntBlock block);
SimEntId Sim_AddEntity(SimScene* scene, f32 x, f32 y, f32 z);
