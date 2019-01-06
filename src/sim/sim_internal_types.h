#pragma once

#include "sim.h"

#define SIM_CMD_TYPE_ADD_ENTITY 1

// BASE FOR ALL COMMANDS
// All commands MUST have a SimCmd as their first member, for
// pointer casting
//#pragma pack(push, 1)
struct SimCmd
{
    // Type and Size must be set by the implementing command
    i32 type;
    i32 size;

    // Must be set from the Sim Scene when created.
    // Controls execution time and order
    u32 tick;
    i32 sequence;
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
