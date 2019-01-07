#pragma once

#include "sim.h"
#include "sim_internal_types.h"

////////////////////////////////////////////////////////////////////
// Command management
////////////////////////////////////////////////////////////////////
internal i32 Sim_EnqueueCommand(SimScene* sim, u8* ptr)
{
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

    return COM_ERROR_NONE;
}

////////////////////////////////////////////////////////////////////
// Entity assignment
////////////////////////////////////////////////////////////////////
internal SimEntity* Sim_FindEntityBySerialNumber(
    SimScene* scene, i32 serialNumber)
{
    for (i32 j = 0; j < scene->maxEnts; ++j)
    {
        SimEntity* ent = &scene->ents[j];
        if (ent->id.serial == serialNumber)
        {
            return ent;
        }
    }
    return NULL;
}

internal i32 Sim_ReserveRemoteEntitySerial(SimScene* scene)
{
    return scene->remoteEntitySequence++;
}

internal i32 Sim_FindFreeSlot(SimScene* scene, i32 forLocalEnt)
{
    i32 halfMax = scene->maxEnts / 2;
    i32 i = forLocalEnt ? halfMax : 0;
    i32 l = forLocalEnt ? scene->maxEnts : halfMax;
    for (i = 0; i < l; ++i)
    {
        SimEntity* ent = &scene->ents[i];
        if (ent->status != SIM_ENT_STATUS_FREE) { continue; }
        return i;
    }
    return -1;
}

internal SimEntity* Sim_GetFreeReplicatedEntity(SimScene* scene, i32 newSerial)
{
    SimEntity* ent = NULL;
    i32 slotIndex = -1;
    slotIndex = Sim_FindFreeSlot(scene, 0);
    if (slotIndex <= -1 ) { return NULL; }

    // config
    ent = &scene->ents[slotIndex];
    ent->status = SIM_ENT_STATUS_IN_USE;
    ent->id.slot.index = (u16)slotIndex;
	ent->id.serial = newSerial;
    return ent;
}

internal SimEntity* Sim_GetFreeLocalEntity(SimScene* scene, i32 newSerial)
{
    SimEntity* ent = NULL;
    i32 slotIndex = -1;
    slotIndex = Sim_FindFreeSlot(scene, 1);
    if (slotIndex <= -1 ) { return NULL; }

    // config
    ent = &scene->ents[slotIndex];
    ent->status = SIM_ENT_STATUS_IN_USE;
    ent->id.slot.index = (u16)slotIndex;
	ent->id.serial = newSerial;
    return ent;
}

// internal SimEntity* Sim_AcquireEntity(SimScene* scene)
// {
    
// }

/*internal SimEntId Sim_ReserveFreeEntity(SimScene* scene)
{
    SimEntBlock* block = NULL;
    i32 slotIndex = -1;

    for (i32 blockIndex = 0; blockIndex < scene->numBlocks; ++blockIndex)
    {
        block = &scene->blocks[blockIndex];
        slotIndex = Sim_FindFreeSlot(block);
        if (slotIndex != -1) { break; }
    }
    if (slotIndex == -1)
    {
        printf("Failed to find free entity slot in %d blocks\n", scene->numBlocks);
		ILLEGAL_CODE_PATH
		return {};
    }

    i32 entityIndex = (scene->blockSize * block->index) + slotIndex;
    SimEntity* ent = &block->ents[slotIndex];
    ent->status = SIM_ENT_STATUS_RESERVED;
    ent->id.slot.index = (u16)entityIndex;
	ent->id.serial = scene->entSequence++;
    printf("SIM Assigning entity id %d/%d seq %d\n",
        ent->id.slot.iteration, ent->id.slot.index, ent->id.serial);
	return ent->id;
}*/

////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////
i32 Sim_CalcEntityArrayBytes(i32 capacity)
{
    return (sizeof(SimEntity) * capacity);
}

i32 Sim_AddEntity(SimScene* scene, SimEntityDef* def)
//i32 Sim_AddEntity(SimScene* scene, f32 x, f32 y, f32 z)
{
    //SimEntId id = Sim_ReserveFreeEntity(scene);
    i32 serial = Sim_ReserveRemoteEntitySerial(scene);

    printf("SIM Enqueue Add entity %d at %.3f, %.3f\n",
        serial, def->pos[0], def->pos[1]);

    SimCmdAddEntity cmd = {};
    Sim_PrepareCommand(scene, (SimCmd*)&cmd);
    Sim_SetAddEntityCmd(&cmd, def);
    u8* addr = (u8*)&cmd;
    Sim_EnqueueCommand(scene, addr);
    return serial;
}

i32 Sim_RemoveEntity(SimScene* scene, i32 serialNumber)
{
    return COM_ERROR_UNKNOWN;
}

////////////////////////////////////////////////////////////////////
// Lifetime
////////////////////////////////////////////////////////////////////
void Sim_InitScene(
    SimScene* scene, 
    ByteBuffer cmdBufferA,
    ByteBuffer cmdBufferB,
    SimEntity* entityMemory,
    i32 entityMemoryCapacity)
{
    *scene = {};
    scene->remoteEntitySequence = 0;
    scene->localEntitySequence = entityMemoryCapacity / 2;
    scene->ents = entityMemory;
    scene->maxEnts = entityMemoryCapacity;
    i32 numBytes = entityMemoryCapacity * sizeof(SimEntity);
    COM_ZeroMemory((u8*)entityMemory, numBytes);

    scene->commands = {};
    scene->commands.swapped = 0;
    scene->commands.a = cmdBufferA;
    scene->commands.b = cmdBufferB;
    Buf_Clear(&cmdBufferA);
    Buf_Clear(&cmdBufferB);
}

/*
Read Inputs
Update Entities -> generate outputs
Step Physics -> generate outputs
*/
i32 Sim_Tick(SimScene* scene, f32 deltaTime)
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
        switch (header->type)
        {
            case SIM_CMD_TYPE_ADD_ENTITY:
            {
                SimCmdAddEntity* cmd = (SimCmdAddEntity*)header;
                
                SimEntity* ent;
                if (cmd->def.isLocal)
                {
                    ent = Sim_GetFreeLocalEntity(scene, cmd->def.serial);
                }
                else
                {
                    ent = Sim_GetFreeReplicatedEntity(scene, cmd->def.serial);
                }
                
                Assert(ent)
                
                ent->status = SIM_ENT_STATUS_IN_USE;
                ent->t.pos.x = cmd->def.pos[0];
                ent->t.pos.y = cmd->def.pos[1];
                ent->t.pos.z = cmd->def.pos[2];

                printf("SIM Add CMD read\n");
            } break;

            default:
            {
                printf("SIM Unknown command type %d\n", header->type);
            } break;
        }
    }
    Buf_Clear(buf);

    return COM_ERROR_NONE;
}
