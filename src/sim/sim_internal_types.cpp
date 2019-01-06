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
    SimCmd* h = (SimCmd*)cmd;
    h->type = SIM_CMD_TYPE_ADD_ENTITY;
    h->size = sizeof(SimCmdAddEntity);
    //cmd->header.size = sizeof(SimCmdAddEntity);
    //cmd->header.type = SIM_CMD_TYPE_ADD_ENTITY;
    printf("Set Header type %d and size %d\n",
        cmd->header.type, cmd->header.size);
    cmd->id = id;
    cmd->pos[0] = x;
    cmd->pos[1] = y;
    cmd->pos[2] = z;
    
    printf("  Addresses header: 0X%X vs command: 0X%X\n",
        (u32)&cmd->header, (u32)cmd
    );
    Assert((u32)&cmd->header == (u32)cmd)
}
