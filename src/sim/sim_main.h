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
    def->serial = Sim_ReserveRemoteEntitySerial(scene, def->isLocal);

    //printf("SIM Enqueue Add entity %d at %.3f, %.3f\n",
    //    def->serial, def->pos[0], def->pos[1]);

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

internal void Sim_BoundaryCheckEnt(SimEntity* ent, Vec3* min, Vec3* max)
{
    Vec3* p = &ent->t.pos;
    if (p->x < min->x) { p->x = min->x; ent->velocity.x = -ent->velocity.x; }
    if (p->x > max->x) { p->x = max->x; ent->velocity.x = -ent->velocity.x; }

    if (p->y < min->y) { p->y = min->y; ent->velocity.y = -ent->velocity.y; }
    if (p->y > max->y) { p->y = max->y; ent->velocity.y = -ent->velocity.y; }

    if (p->z < min->z) { p->z = min->z; ent->velocity.z = -ent->velocity.z; }
    if (p->z > max->z) { p->z = max->z; ent->velocity.z = -ent->velocity.z; }
}

internal i32 Sim_RunFrame(SimScene* scene, f32 deltaTime)
{
    for (i32 i = 0; i < scene->maxEnts; ++i)
    {
        SimEntity* ent = &scene->ents[i];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }
        Vec3* pos = &ent->t.pos;
        ent->previousPos.x = pos->x;
        ent->previousPos.y = pos->y;
        ent->previousPos.z = pos->z;
        Vec3 move =
        {
            ent->velocity.x * deltaTime,
            ent->velocity.y * deltaTime,
            ent->velocity.z * deltaTime
        };
        
        ent->t.pos.x += move.x;
        ent->t.pos.y += move.y;
        ent->t.pos.z += move.z;
        
        Sim_BoundaryCheckEnt(ent, &scene->boundaryMin, &scene->boundaryMax);

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
            #if 0
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
            //printf("SIM CMD %d Add ent %d\n",
            //    cmd->header.sequence, cmd->def.serial);
            
            ent->status = SIM_ENT_STATUS_IN_USE;
            ent->t.pos.x = cmd->def.pos[0];
            ent->t.pos.y = cmd->def.pos[1];
            ent->t.pos.z = cmd->def.pos[2];
            ent->previousPos.x = cmd->def.pos[0];
            ent->previousPos.x = cmd->def.pos[1];
            ent->previousPos.x = cmd->def.pos[2];
            ent->velocity.x = cmd->def.velocity[0];
            ent->velocity.y = cmd->def.velocity[1];
            ent->velocity.z = cmd->def.velocity[2];
            //printf("V: %.3f, %.3f, %.3f\n",
            //    ent->velocity.x, ent->velocity.y, ent->velocity.z);
            i32 badScale = 0;
            if (cmd->def.scale[0] == 0
                || cmd->def.scale[1] == 0
                || cmd->def.scale[2] == 0)
            {
                //printf("  bad scale\n");
                badScale = 1;
            }
            if (!badScale)
            {
                ent->t.scale.x = cmd->def.scale[0];
                ent->t.scale.y = cmd->def.scale[1];
                ent->t.scale.z = cmd->def.scale[2];
            }
            else
            {
                ent->t.scale.x = 1;
                ent->t.scale.y = 1;
                ent->t.scale.z = 1;
            }
            #endif
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
