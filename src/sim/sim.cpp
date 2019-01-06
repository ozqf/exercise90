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
    Assert(cmd->type != 0)
    Assert(cmd->size > 0)

    ByteBuffer* buf = sim->commands.GetWrite();
	printf("SIM Writing command at 0X%X\n", (u32)buf->ptrWrite);

	// Via old macro... DOESN'T WORK
    //buf->ptrWrite += COM_COPY(&cmd, buf->ptrWrite, cmd->size);

	// copy memory called... does...?
	buf->ptrWrite += COM_CopyMemory(ptr, buf->ptrWrite, cmd->size);

	// Via pointer assignment - works
	//*(SimCmdAddEntity*)buf->ptrWrite = *addEnt;
	//buf->ptrWrite += addEnt->header.size;
    printf("SIM Enqueued type %d size %d. Wrote %d bytes of cmds\n", cmd->type, cmd->size, buf->Written());

    return COM_ERROR_NONE;
}

////////////////////////////////////////////////////////////////////
// Entity assignment
////////////////////////////////////////////////////////////////////
internal i32 Sim_FindFreeSlot(SimEntBlock* block)
{
    for (i32 i = 0; i < block->capacity; ++i)
    {
        SimEntity* ent = &block->ents[i];
        if (ent->status != SIM_ENT_STATUS_FREE) { continue; }
        return i;
    }
    return -1;
}

internal SimEntId Sim_ReserveFreeEntity(SimScene* scene)
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
	ent->id.sequence = scene->entSequence++;
    printf("SIM Assigning entity id %d/%d seq %d\n",
        ent->id.slot.iteration, ent->id.slot.index, ent->id.sequence);
	return ent->id;
}

SimEntId Sim_AddEntity(SimScene* scene, f32 x, f32 y, f32 z)
{
    SimEntId id = Sim_ReserveFreeEntity(scene);

    //SimEntity* ent = Sim_GetFreeEntity(scene);
    //ent->t.pos.x = x;
    //ent->t.pos.y = y;
    //ent->t.pos.z = z;
    //printf("SIM Added entity %d at %.3f, %.3f\n",
    //    ent->id.sequence, ent->t.pos.x, ent->t.pos.y);

    SimCmdAddEntity cmd = {};
    Sim_PrepareCommand(scene, (SimCmd*)&cmd);
    Sim_SetAddEntityCmd(&cmd, id, x, y, z);
	u8* addr = (u8*)&cmd;
    Sim_EnqueueCommand(scene, addr);
    return id;
}

void Sim_AddEntBlock(SimScene* scene, SimEntBlock block)
{
    Assert(scene->blockSize == block.capacity)

    i32 i = scene->numBlocks;
    i32 last = scene->maxBlocks - 1; 
    if (i >= last)
    {
        ILLEGAL_CODE_PATH
        return;
    }
    scene->blocks[i] = block;
    scene->blocks[i].index = scene->numBlocks;
    scene->numBlocks++;
    for (i32 j = 0; j < block.capacity; ++j)
    {
        SimEntity* ent = &block.ents[j];
        *ent = {};
        ent->id.slot.iteration = 1;
    }
    printf("SIM new block added - now at %d blocks\n", scene->numBlocks);
}

////////////////////////////////////////////////////////////////////
// Lifetime
////////////////////////////////////////////////////////////////////
void Sim_InitScene(
    SimScene* scene, ByteBuffer cmdBufferA, ByteBuffer cmdBufferB)
{
    *scene = {};
    scene->maxBlocks = SIM_ENT_MAX_BLOCKS;
    scene->blockSize = SIM_ENT_BLOCK_SIZE;
    scene->commands = {};
    scene->commands.swapped = 0;
    scene->commands.a = cmdBufferA;
    scene->commands.b = cmdBufferB;
    Buf_Clear(&cmdBufferA);
    Buf_Clear(&cmdBufferB);
}

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
