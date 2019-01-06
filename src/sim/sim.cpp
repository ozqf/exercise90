#pragma once

#include "sim.h"

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

internal SimEntity* Sim_GetFreeEntity(SimScene* scene)
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
		return NULL;
    }

    i32 entityIndex = (scene->blockSize * block->index) + slotIndex;
    SimEntity* ent = &block->ents[slotIndex];
    ent->status = SIM_ENT_STATUS_IN_USE;
    ent->id.slot.index = (u16)entityIndex;
	ent->id.sequence = scene->nextEntId++;
    printf("SIM Assigning entity id %d/%d seq %d\n",
        ent->id.slot.iteration, ent->id.slot.index, ent->id.sequence);
	return ent;
}

SimEntId Sim_AddEntity(SimScene* scene, f32 x, f32 y, f32 z)
{
    SimEntity* ent = Sim_GetFreeEntity(scene);
    ent->t.pos.x = x;
    ent->t.pos.y = y;
    ent->t.pos.z = z;
    printf("SIM Added entity %d at %.3f, %.3f\n",
        ent->id.sequence, ent->t.pos.x, ent->t.pos.y);
    return ent->id;
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
void Sim_InitScene(SimScene* scene)
{
    *scene = {};
    scene->maxBlocks = SIM_ENT_MAX_BLOCKS;
    scene->blockSize = SIM_ENT_BLOCK_SIZE;
}

void Sim_Tick(SimScene* scene, f32 deltaTime)
{
    
}
