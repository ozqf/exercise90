#pragma once

#include "sim.h"

#define SIM_CMD_TYPE_NULL 0
#define SIM_CMD_TYPE_ADD_ENTITY 1
#define SIM_CMD_TYPE_REMOVE_ENTITY 2

// 48879?
#define SIM_CMD_SENTINEL 0xBEEF

// BASE FOR ALL COMMANDS
// All commands MUST have a SimCmd as their first member, for
// pointer casting
struct SimCmd
{
    // for alignment checking
    i32 sentinel;
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
    SimEntityDef def;
};

struct SimCmdRemoveEntity
{
    SimCmd header;
    i32 serial;
};

struct SimCmdSyncEntity
{
    SimCmd header;
    i32 serial;
	f32 pos[3];
};

struct SimCmdProjectile
{
	SimCmd header;
	f32 origin[3];
	f32 velocity[3];
	i32 deathTick;
};

void Sim_PrepareCommand(SimScene* scene, SimCmd* cmd);
void Sim_SetAddEntityCmd(SimCmdAddEntity* cmd, SimEntityDef* def);
