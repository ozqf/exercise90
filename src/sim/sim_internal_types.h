#pragma once

#include "sim.h"

#define SIM_CMD_TYPE_ADD_ENTITY 1

//#pragma pack(push, 1)
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
    i32 serial;
    f32 pos[3];
};
//#pragma pack(pop)

void Sim_PrepareCommand(SimScene* scene, SimCmd* cmd);
void Sim_SetAddEntityCmd(SimCmdAddEntity* cmd, i32 id, f32 x, f32 y, f32 z);
