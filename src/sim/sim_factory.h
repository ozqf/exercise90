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

internal SimEntity* Sim_GetFreeLocalEntity(
    SimScene* scene, i32 newSerial)
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
internal void Sim_InitEntity(SimEntity* ent, SimEntityDef* def)
{
    ent->birthTick = def->birthTick;
    ent->destination = def->destination;
    //ent->deathTick = def->deathTick;
	
    ent->t.pos =          def->pos;
    ent->previousPos =    def->pos;
    ent->velocity =         def->velocity;


    i32 badScale = 0;
    if (def->scale.x == 0
        || def->scale.y == 0
        || def->scale.z == 0)
    {
        //printf("  bad scale\n");
        badScale = 1;
    }
    if (!badScale)
    {
        ent->t.scale = def->scale;
    }
    else
    {
        ent->t.scale.x = 1;
        ent->t.scale.y = 1;
        ent->t.scale.z = 1;
    }
}

internal i32 Sim_InitActor(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_InitEntity(ent, def);
    ent->tickType = SIM_TICK_TYPE_ACTOR;
    ent->attackTime = 0.2f;
    ent->display.colour = { 0, 1, 0, 1 };
    return COM_ERROR_NONE;
}

internal i32 Sim_InitWanderer(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_InitEntity(ent, def);
    ent->speed = 3;
    ent->tickType = SIM_TICK_TYPE_WANDERER;
    ent->display.colour = { 0.2f, 0.2f, 1, 1 };
    return COM_ERROR_NONE;
}

internal i32 Sim_InitWorldVolume(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_InitEntity(ent, def);
    APP_PRINT(256, "SIM Spawning world volume at %.3f, %.3f, %.3f\n",
        def->pos.x, def->pos.y, def->pos.z);
    ent->tickType = SIM_TICK_TYPE_WORLD;
    // world volumes can't move (yet!)
    ent->velocity = {};
    ent->display.colour = { 0.2f, 0.2f, 0.2f, 1 };
    return COM_ERROR_NONE;
}

internal i32 Sim_InitTurret(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_InitEntity(ent, def);
    ent->tickType = SIM_TICK_TYPE_TURRET;
    ent->thinkTime = 4;//1.25f;
	ent->lifeTime = 10;
    printf("Spawned turret, Ticktype %d Vel %.3f, %.3f, %.3f\n",
        ent->tickType, ent->velocity.x, ent->velocity.y, ent->velocity.z);
    return COM_ERROR_NONE;
}

internal i32 Sim_InitLineTrace(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
	//printf("SIM Create line trace\n");
    Sim_InitEntity(ent, def);
    ent->tickType = SIM_TICK_TYPE_LINE_TRACE;
	ent->lifeTime = 1.0f;
    return COM_ERROR_NONE;
}

internal i32 Sim_InitProjBase(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    SimProjectileType t;
    t.speed = 10.0f;
    t.patternDef.numItems = 1;
    t.lifeTime = 2.0f;
    t.patternDef.patternId = SIM_PATTERN_NONE;
    t.scale = { 1, 1, 1 };

    Sim_InitProjectile(
        ent,
        &def->pos,
        &def->velocity,
        &t,
        def->fastForwardTicks);

    return COM_ERROR_NONE;
}

internal i32 Sim_InitProjPrediction(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    SimProjectileType t;
    t.speed = 10.0f;
    t.patternDef.numItems = 1;
    t.lifeTime = 2.0f;
    t.patternDef.patternId = SIM_PATTERN_NONE;
    t.scale = { 0.5f, 0.5f, 0.5f };

    Sim_InitProjectile(
        ent,
        &def->pos,
        &def->velocity,
        &t,
        def->fastForwardTicks);

    return COM_ERROR_NONE;
}

internal i32 Sim_InitProjTest(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    SimProjectileType t;
    t.speed = 10.0f;
    t.patternDef.numItems = 4;
    t.lifeTime = 6.0f;
    t.patternDef.patternId = SIM_PATTERN_RADIAL;
    t.scale = { 1, 1, 1 };

    Sim_InitProjectile(
        ent,
        &def->pos,
        &def->velocity,
        &t,
        def->fastForwardTicks);

    return COM_ERROR_NONE;
}

internal SimEntity* Sim_SpawnEntity(
    SimScene* sim, SimEntityDef* def)
{
    SimEntity* ent;
    if (def->isLocal)
    {
        ent = Sim_GetFreeLocalEntity(sim, def->serial);
    }
    else
    {
        ent = Sim_GetFreeReplicatedEntity(sim, def->serial);
    }
    if (!ent)
    {
        APP_PRINT(64, "SIM No Free Entity Available for Ent %d!\n",
            def->serial
        );
    }
    
    Assert(ent)
    ent->status = SIM_ENT_STATUS_IN_USE;
    // Record factory type so we know how this entity was initialised
    ent->factoryType = def->factoryType;

    ErrorCode err;

    switch (def->factoryType)
    {
        case SIM_FACTORY_TYPE_PROJECTILE_BASE:
            err =  Sim_InitProjBase(sim, ent, def); break;
        case SIM_FACTORY_TYPE_PROJ_PREDICTION:
            err =  Sim_InitProjPrediction(sim, ent, def); break;
        case SIM_FACTORY_TYPE_PROJ_TEST:
            err =  Sim_InitProjTest(sim, ent, def); break;
        case SIM_FACTORY_TYPE_ACTOR:
            err =  Sim_InitActor(sim, ent, def); break;
        case SIM_FACTORY_TYPE_WANDERER:
            err =  Sim_InitWanderer(sim, ent, def); break;
        case SIM_FACTORY_TYPE_WORLD:
            err =  Sim_InitWorldVolume(sim, ent, def); break;
        case SIM_FACTORY_TYPE_TURRET:
			err = Sim_InitTurret(sim, ent, def); break;
        case SIM_FACTORY_TYPE_LINE_TRACE:
		    err =  Sim_InitLineTrace(sim, ent, def); break;
		case SIM_FACTORY_TYPE_NONE:
        {
            printf("SIM Cannot spawn, entity type not set!\n");
            
            return NULL;
        } break;

        default:
        {
            printf("SIM Unknown entity type %d\n", def->factoryType);
            ILLEGAL_CODE_PATH
            return NULL;
        } break;
    }

    return ent;
}

internal i32 Sim_RecycleEntity(
    SimScene* sim, i32 entitySerialNumber)
{
    SimEntity* ent = Sim_FindEntityBySerialNumber(
        sim, entitySerialNumber);
    if (ent)
    {
        SimEntIndex slot = ent->id.slot;
        APP_LOG(64, "SIM Removing ent %d (slot %d/%d)\n",
            entitySerialNumber, slot.iteration, slot.index);
        u16 iteration = ent->id.slot.iteration + 1;
        *ent = {};
        ent->id.slot.iteration = iteration;
        return COM_ERROR_NONE;
    }
    else
    {
        APP_LOG(64, "SIM Found no ent %d to remove\n",
            entitySerialNumber);
        return COM_ERROR_BAD_ARGUMENT;
    }
}
