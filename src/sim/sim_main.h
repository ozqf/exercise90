#pragma once

#include "sim.h"

////////////////////////////////////////////////////////////////////
// Command management
////////////////////////////////////////////////////////////////////
internal i32 Sim_EnqueueCommand(SimScene* sim, u8* ptr)
{
    // immediate mode
    #if 1
    Sim_Execute(sim, (SimCmd*)ptr);
    #endif
    #if 0
	SimCmd* cmd = (SimCmd*)ptr;
	//SimCmdAddEntity* addEnt = (SimCmdAddEntity*)ptr;
    Assert(cmd)
    Assert(cmd->sentinel == SIM_CMD_SENTINEL)
    Assert(cmd->type != 0)
    Assert(cmd->size > 0)

    ByteBuffer* buf = sim->commands.GetWrite();
	//printf("SIM Writing command at 0X%X\n", (u32)buf->ptrWrite);

	// Via old macro... DOESN'T WORK
    //buf->ptrWrite += COM_COPY(&cmd, buf->ptrWrite, cmd->size);

	// copy memory called... does...?
	buf->ptrWrite += COM_CopyMemory(ptr, buf->ptrWrite, cmd->size);

	// Via pointer assignment - works
	//*(SimCmdAddEntity*)buf->ptrWrite = *addEnt;
	//buf->ptrWrite += addEnt->header.size;
    //printf("SIM Enqueued type %d size %d. Wrote %d bytes of cmds\n",
    //    cmd->type, cmd->size, buf->Written());
    #endif
    return COM_ERROR_NONE;
}

////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////
i32 Sim_CalcEntityArrayBytes(i32 capacity)
{
    return (sizeof(SimEntity) * capacity);
}

i32 Sim_AddEntity(SimScene* scene, SimEntityDef* def)
{
    def->serial = Sim_ReserveRemoteEntitySerial(scene, def->isLocal);
    SimCmdAddEntity cmd = {};
    Sim_PrepareCommand(scene, (SimCmd*)&cmd);
    Sim_SetAddEntityCmd(&cmd, def);
    u8* addr = (u8*)&cmd;
    Sim_EnqueueCommand(scene, addr);
    return def->serial;
}

i32 Sim_RemoveEntity(SimScene* scene, i32 serialNumber)
{
	SimCmdRemoveEntity cmd = {};
	Sim_PrepareCommand(scene, &cmd.header);
	cmd.serial = serialNumber;
	Sim_EnqueueCommand(scene, (u8*)&cmd);
    return COM_ERROR_NONE;
}

////////////////////////////////////////////////////////////////////
// Lifetime
////////////////////////////////////////////////////////////////////
void Sim_InitScene(
    SimScene* scene, 
    ByteBuffer cmdBufferA,
    ByteBuffer cmdBufferB,
    SimEntity* entityMemory,
    i32 maxEntities)
{
    *scene = {};
    scene->remoteEntitySequence = 0;
    scene->localEntitySequence = maxEntities / 2;
    scene->ents = entityMemory;
    scene->maxEnts = maxEntities;
    i32 numBytes = maxEntities * sizeof(SimEntity);
    COM_ZeroMemory((u8*)entityMemory, numBytes);

    scene->commands = {};
    scene->commands.swapped = 0;
    scene->commands.a = cmdBufferA;
    scene->commands.b = cmdBufferB;
    Buf_Clear(&cmdBufferA);
    Buf_Clear(&cmdBufferB);
}

internal i32 Sim_RunFrame(SimScene* scene, f32 deltaTime)
{
    for (i32 i = 0; i < scene->maxEnts; ++i)
    {
        SimEntity* ent = &scene->ents[i];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }
        Sim_UpdateEntity(scene, ent, deltaTime);
    }
    return COM_ERROR_NONE;
}

i32 Sim_Execute(SimScene* scene, SimCmd* header)
{
    if (header->type == 0)
    {
        printf("SIM ABORT bad cmd type %d\n", header->type);
        return COM_ERROR_UNKNOWN_COMMAND;
    }
    if (header->size == 0)
    {
        printf("SIM ABORT bad cmd size %d\n", header->size);
        return COM_ERROR_BAD_SIZE;
    }
    if (header->sentinel != SIM_CMD_SENTINEL)
    {
        printf("SIM ABORT cmd sentinel mismatch\n");
        return COM_ERROR_BAD_ARGUMENT;
    }
    switch (header->type)
    {
        case SIM_CMD_TYPE_ADD_ENTITY:
        {
            SimCmdAddEntity* cmd = (SimCmdAddEntity*)header;
			Assert(cmd->header.size == sizeof(SimCmdAddEntity));
            return Sim_SpawnEntity(scene, header, &cmd->def);
        } break;
		
		case SIM_CMD_TYPE_REMOVE_ENTITY:
		{
			SimCmdRemoveEntity* cmd = (SimCmdRemoveEntity*)header;
			Assert(cmd->header.size == sizeof(SimCmdRemoveEntity));
			printf("SIM Removing ent %d\n", cmd->serial);
		} break;
        default:
        {
            printf("SIM Unknown command type %d\n", header->type);
        } break;
    }
    return COM_ERROR_NONE;
}
#if 0
internal i32 Sim_ReadInput(SimScene* scene, f32 deltaTime)
{
    scene->commands.Swap();
    ByteBuffer* buf = scene->commands.GetRead();
	if (buf->Written() == 0)
	{
		return COM_ERROR_NONE;
	}
    printf("SIM tick reading from 0X%X (%d bytes)\n", (u32)buf->ptrStart, buf->Written());

    u8* read = buf->ptrStart;
    u8* end = buf->ptrWrite;
    while (read < end)
    {
        SimCmd* header = (SimCmd*)read;
        read += header->size;
        Sim_Execute(scene, header);
    }
    Buf_Clear(buf);
    return COM_ERROR_NONE;
}
#endif
/*
Read Inputs
Update Entities -> generate outputs
Step Physics -> generate outputs
*/
i32 Sim_Tick(SimScene* scene, f32 deltaTime)
{
    //Sim_ReadInput(scene, deltaTime);
    Sim_RunFrame(scene, deltaTime);
    return COM_ERROR_NONE;
}
