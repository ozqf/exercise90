#pragma once

#include "sim.h"

#define SIM_CMD_TYPE_ADD_ENTITY 1

struct SimCmd
{
    i32 type;
    u32 tick;
    i32 sequence;
    i32 size;
};

struct SimCmdAddEntity
{
    SimCmd header;
    SimEntId id;
    f32 pos[3];
};

void Sim_PrepareCommand(SimScene* scene, SimCmd* cmd);
void Sim_SetAddEntityCmd(SimCmdAddEntity* cmd, SimEntId id, f32 x, f32 y, f32 z);
