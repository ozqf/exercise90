#pragma once

#include "../common/com_module.h"

#include "sim.h"
#include "sim_internal_types.h"

void Sim_PrepareCommand(SimScene* scene, SimCmd* cmd);
void Sim_SetAddEntityCmd(SimCmdAddEntity* cmd, SimEntId id, f32 x, f32 y, f32 z);

#include "sim.cpp"
#include "sim_internal_types.cpp"
