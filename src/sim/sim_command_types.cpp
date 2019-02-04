#pragma once
#if 0
#include "sim.h"

void Sim_PrepareCommand(SimScene* scene, SimCmd* cmd)
{
    cmd->sentinel = SIM_CMD_SENTINEL;
    cmd->sequence = scene->cmdSequence++;
    cmd->tick = scene->tick;
    // Set type and size to 0. These can only be known by the concrete
    // command type. if a type or size of 0 is read, error immediately.
    // This should enforce the concrete init being run when this cmd
    // is placed in the queue.
    cmd->type = 0;
    cmd->size = 0;
}

//void Sim_SetAddEntityCmd(SimCmdAddEntity* cmd, i32 id, f32 x, f32 y, f32 z)
void Sim_SetAddEntityCmd(SimCmdAddEntity* cmd, SimEntityDef* def)
{
    SimCmd* h = (SimCmd*)cmd;
    h->type = SIM_CMD_TYPE_ADD_ENTITY;
    h->size = sizeof(SimCmdAddEntity);
    cmd->def = *def;
    Assert((u32)&cmd->header == (u32)cmd)
}
#endif
