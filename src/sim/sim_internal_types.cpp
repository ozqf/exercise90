#pragma once

#include "sim.h"
#include "sim_internal_types.h"

void Sim_PrepareCommand(SimScene* scene, SimCmd* cmd)
{
    cmd->sequence = scene->cmdSequence++;
    cmd->tick = scene->tick;
    // Set type and size to 0. These can only be known by the concrete
    // command type. if a type or size of 0 is read, error immediately.
    // This should enforce the concrete init being run when this cmd
    // is placed in the queue.
    cmd->type = 0;
    cmd->size = 0;
}

void Sim_SetAddEntityCmd(SimCmdAddEntity* cmd, SimEntId id, f32 x, f32 y, f32 z)
{
    cmd->header.size = sizeof(SimCmdAddEntity);
    cmd->header.type = SIM_CMD_TYPE_ADD_ENTITY;
    cmd->id = id;
    cmd->pos[0] = x;
    cmd->pos[1] = y;
    cmd->pos[2] = z;
}
