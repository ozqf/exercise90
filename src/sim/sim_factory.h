#pragma once

#include "sim.h"

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

internal i32 Sim_FreeEntityBySerial(SimScene* scene, i32 serial)
{
	for (i32 i = 0; i < scene->maxEnts; ++i)
	{
		SimEntity* ent = &scene->ents[i];
		
		if (ent->id.serial != serial) { continue; }
		
		// free slot
		ent->status = SIM_ENT_STATUS_FREE;
		
		return COM_ERROR_NONE;
	}
	return COM_ERROR_NOT_FOUND;
}

internal i32 Sim_FindFreeSlot(SimScene* scene, i32 forLocalEnt)
{
    i32 halfMax = scene->maxEnts / 2;
    i32 i = forLocalEnt ? halfMax : 0;
    i32 l = forLocalEnt ? scene->maxEnts : halfMax;
    for (; i < l; ++i)
    {
        SimEntity* ent = &scene->ents[i];
        if (ent->status != SIM_ENT_STATUS_FREE) { continue; }
        return i;
    }
    return -1;
}

internal SimEntity* Sim_GetFreeReplicatedEntity(
    SimScene* scene, i32 newSerial)
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
	APP_LOG(64,
        "SIM assigned replicated ent serial %d (slot %d/%d)\n",
        ent->id.serial, ent->id.slot.iteration, ent->id.slot.index);
    ent->isLocal = 0;
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
	APP_LOG(64, "SIM assigned local ent serial %d (slot %d/%d)\n",
        ent->id.serial, ent->id.slot.iteration, ent->id.slot.index);
    ent->isLocal = 1;
    return ent;
}

////////////////////////////////////////////////////////////////////
// Entity initialisation
////////////////////////////////////////////////////////////////////
internal void Sim_ApplySpawnTransform(SimEntity* ent, SimEntityDef* def)
{
    ent->birthTick = def->birthTick;
    //ent->deathTick = def->deathTick;
	
    ent->t.pos.x =          def->pos[0];
    ent->t.pos.y =          def->pos[1];
    ent->t.pos.z =          def->pos[2];
    ent->previousPos.x =    def->pos[0];
    ent->previousPos.y =    def->pos[1];
    ent->previousPos.z =    def->pos[2];
    ent->velocity.x =       def->velocity[0];
    ent->velocity.y =       def->velocity[1];
    ent->velocity.z =       def->velocity[2];


    i32 badScale = 0;
    if (def->scale[0] == 0
        || def->scale[1] == 0
        || def->scale[2] == 0)
    {
        //printf("  bad scale\n");
        badScale = 1;
    }
    if (!badScale)
    {
        ent->t.scale.x = def->scale[0];
        ent->t.scale.y = def->scale[1];
        ent->t.scale.z = def->scale[2];
    }
    else
    {
        ent->t.scale.x = 1;
        ent->t.scale.y = 1;
        ent->t.scale.z = 1;
    }
}

internal i32 Sim_InitActor(SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_ApplySpawnTransform(ent, def);
    ent->entType = def->entType;
    ent->attackTime = 0.2f;
    return COM_ERROR_NONE;
}

internal i32 Sim_InitWanderer(SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_ApplySpawnTransform(ent, def);
    ent->entType = def->entType;
    return COM_ERROR_NONE;
}

internal i32 Sim_InitWorldVolume(SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_ApplySpawnTransform(ent, def);
    APP_PRINT(256, "SIM Spawning world volume at %.3f, %.3f, %.3f\n",
        def->pos[0], def->pos[1], def->pos[2]);
    ent->entType = def->entType;
    // world volumes can't move (yet!)
    ent->velocity = {};
    return COM_ERROR_NONE;
}

internal i32 Sim_InitTurret(SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_ApplySpawnTransform(ent, def);
    ent->entType = def->entType;
    ent->thinkTime = 4;//1.25f;
	ent->lifeTime = 10;
    return COM_ERROR_NONE;
}

internal i32 Sim_SpawnEntity(SimScene* scene, SimEntityDef* def)
{
    SimEntity* ent;
    if (def->isLocal)
    {
        ent = Sim_GetFreeLocalEntity(scene, def->serial);
    }
    else
    {
        ent = Sim_GetFreeReplicatedEntity(scene, def->serial);
    }
    if (!ent)
    {
        APP_PRINT(64, "SIM No Free Entity Available for Ent %d!\n",
            def->serial
        );
    }
    
    Assert(ent)
    ent->status = SIM_ENT_STATUS_IN_USE;

    switch (def->entType)
    {
        case SIM_ENT_TYPE_ACTOR:
        {
            return Sim_InitActor(scene, ent, def);
        } break;
        case SIM_ENT_TYPE_WANDERER:
        {
            return Sim_InitWanderer(scene, ent, def);
        }
        case SIM_ENT_TYPE_WORLD:
        {
            return Sim_InitWorldVolume(scene, ent, def);
        }
        case SIM_ENT_TYPE_TURRET:
        {
            return Sim_InitTurret(scene, ent, def);
        }
        case SIM_ENT_TYPE_NONE:
        {
            printf("SIM Cannot spawn, entity type not set!\n");
            ILLEGAL_CODE_PATH
            return COM_ERROR_BAD_ARGUMENT;
        } break;

        default:
        {
            printf("SIM Unknown entity type %d\n", def->entType);
            ILLEGAL_CODE_PATH
            return COM_ERROR_BAD_ARGUMENT;
        } break;
    }

    //return COM_ERROR_NONE;
}

internal i32 Sim_RecycleEntity(SimScene* sim, i32 entitySerialNumber)
{
    SimEntity* ent = Sim_FindEntityBySerialNumber(sim, entitySerialNumber);
    if (ent)
    {
        SimEntIndex slot = ent->id.slot;
        APP_LOG(64, "SIM Removing ent %d (slot %d/%d)\n",
            entitySerialNumber, slot.iteration, slot.index);
        
        *ent = {};
        ent->id.slot.iteration++;
        return COM_ERROR_NONE;
    }
    else
    {
        APP_PRINT(64, "SIM Found no ent %d to remove\n", entitySerialNumber);
        return COM_ERROR_BAD_ARGUMENT;
    }
}
