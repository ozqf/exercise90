#pragma once

/*
> Manages SimEntity pool
> Performs new entity configuration
*/
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
    // For clients, keep track of the latest entity spawned.
    if (newSerial > scene->highestAssignedSequence)
    {
        scene->highestAssignedSequence = newSerial;
    }
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
internal i32 Sim_InitActor(
    SimScene* scene, SimEntity* ent, SimEntSpawnData* def)
{
    Sim_SetEntityBase(ent, def);
    ent->tickType = SIM_TICK_TYPE_ACTOR;
    ent->coreTickType = SIM_TICK_TYPE_ACTOR;
    Sim_SetEntityStats(ent, 6.5f, 1, 0.05f);
    Sim_SetEntityDisplay(ent,
        { 0, 1, 0, 1 },
        { 0, 1, 0, 1 },
        COM_MESH_CUBE_INDEX,
        SIM_DEATH_GFX_EXPLOSION);
    //ent->flags = SIM_ENT_FLAG_POSITION_SYNC;
    return COM_ERROR_NONE;
}

internal i32 Sim_InitBot(
    SimScene* scene, SimEntity* ent, SimEntSpawnData* def)
{
    Sim_SetEntityBase(ent, def);
    ent->tickType = SIM_TICK_TYPE_BOT;
    ent->coreTickType = SIM_TICK_TYPE_BOT;
    Sim_SetEntityStats(ent, 6.5f, 1, 0.05f);
    Sim_SetEntityDisplay(ent,
        { 0, 0.6f, 0, 1 },
        { 0, 0.6f, 0, 1 },
        COM_MESH_CUBE_INDEX,
        SIM_DEATH_GFX_EXPLOSION);
    //ent->flags = SIM_ENT_FLAG_POSITION_SYNC;
    return COM_ERROR_NONE;
}

internal i32 Sim_InitWorldVolume(
    SimScene* scene, SimEntity* ent, SimEntSpawnData* def)
{
    Sim_SetEntityBase(ent, def);
    APP_PRINT(256, "SIM Spawning world volume at %.3f, %.3f, %.3f\n",
        def->pos.x, def->pos.y, def->pos.z);
    ent->tickType = SIM_TICK_TYPE_WORLD;
    ent->coreTickType = SIM_TICK_TYPE_WORLD;
    // world volumes can't move (yet!)
    ent->body.velocity = {};
    Sim_SetEntityDisplay(ent,
        { 0.2f, 0.2f, 0.2f, 1 },
        { 0.2f, 0.2f, 0.2f, 1 },
        COM_MESH_CUBE_INDEX,
        SIM_DEATH_GFX_NONE);
    #ifdef SIM_USE_PHYSICS_ENGINE
    ent->shape.SetAsBox(def->pos, def->scale, ZCOLLIDER_FLAG_STATIC, SIM_LAYER_WORLD, SIM_LAYER_WORLD, 0);
    PhysCmd_CreateShape(scene->world, &ent->shape, ent->id.serial);
    #endif
    return COM_ERROR_NONE;
}

internal i32 Sim_InitSpawner(
    SimScene* scene, SimEntity* ent, SimEntSpawnData* def)
{
    Sim_SetEntityBase(ent, def);
    ent->tickType = SIM_TICK_TYPE_SPAWNER;
    ent->coreTickType = SIM_TICK_TYPE_SPAWNER;
    ent->relationships.childSpawnCount = def->numChildren;
    ent->relationships.maxLiveChildren = def->numChildren;
    ent->relationships.totalChildren = def->numChildren;
    //printf("Spawned Spawner, Ticktype %d Vel %.3f, %.3f, %.3f\n",
    //    ent->tickType,
    //    ent->body.velocity.x,
    //    ent->body.velocity.y,
    //    ent->body.velocity.z);
    return COM_ERROR_NONE;
}

internal i32 Sim_InitLineTrace(
    SimScene* scene, SimEntity* ent, SimEntSpawnData* def)
{
	//printf("SIM Create line trace\n");
    Sim_SetEntityBase(ent, def);
    ent->tickType = SIM_TICK_TYPE_LINE_TRACE;
    ent->coreTickType = SIM_TICK_TYPE_LINE_TRACE;
    ent->timing.nextThink = ent->timing.birthTick + App_CalcTickInterval(2);
    return COM_ERROR_NONE;
}

///////////////////////////////////////////////////////
// GFX
///////////////////////////////////////////////////////
internal i32 Sim_InitExplosion(
    SimScene* sim, SimEntity* ent, SimEntSpawnData* def)
{
    Sim_SetEntityBase(ent, def);
    ent->tickType = SIM_TICK_TYPE_EXPLOSION;
    ent->coreTickType = SIM_TICK_TYPE_EXPLOSION;
    Sim_SetEntityDisplay(ent,
        { 1, 1, 0, 1 },
        { 1, 1, 0, 1 },
        COM_MESH_CUBE_INDEX,
        SIM_DEATH_GFX_NONE);
    ent->timing.nextThink = ent->timing.birthTick + App_CalcTickInterval(0.5f);
    ent->body.t.scale = { 2, 1, 2 };
    return COM_ERROR_NONE;
}

///////////////////////////////////////////////////////
// Projectiles
///////////////////////////////////////////////////////
internal i32 Sim_InitProjBase(
    SimScene* scene, SimEntity* ent, SimEntSpawnData* def)
{
    SimProjectileType t;
    t.speed = 10.0f;
    t.patternDef.numItems = 1;
    t.lifeTime = 1.0f;
    t.patternDef.patternId = SIM_PATTERN_NONE;
    t.scale = { 0.5f, 0.5f, 0.5f };
    Sim_SetEntityDisplay(ent,
        { 1, 1, 0, 1 },
        { 1, 1, 0, 1 },
        COM_MESH_SPIKE_INDEX,
        SIM_DEATH_GFX_EXPLOSION);
    // must set birth tick here
    ent->timing.birthTick = def->birthTick;

    Sim_InitProjectile(
        ent,
        &def->pos,
        &def->velocity,
        &t,
        def->fastForwardTicks);

    return COM_ERROR_NONE;
}

internal i32 Sim_InitProjPrediction(
    SimScene* scene, SimEntity* ent, SimEntSpawnData* def)
{
    SimProjectileType t;
    t.speed = 45.0f;
    t.patternDef.numItems = 1;
    t.lifeTime = 2.0f;
    t.patternDef.patternId = SIM_PATTERN_NONE;
    t.scale = { 0.5f, 0.5f, 0.5f };
    Sim_SetEntityDisplay(ent,
        { 1, 1, 0, 1 },
        { 1, 1, 0, 1 },
        COM_MESH_SPIKE_INDEX,
        SIM_DEATH_GFX_EXPLOSION);
    // must set birth tick here
    ent->timing.birthTick = def->birthTick;

    Sim_InitProjectile(
        ent,
        &def->pos,
        &def->velocity,
        &t,
        def->fastForwardTicks);

    return COM_ERROR_NONE;
}

internal i32 Sim_InitProjTest(
    SimScene* scene, SimEntity* ent, SimEntSpawnData* def)
{
    SimProjectileType t;
    t.speed = 45.0f;
    t.patternDef.numItems = 4;
    t.lifeTime = 1.5f;
    t.patternDef.patternId = SIM_PATTERN_RADIAL;
    t.scale = { 1, 1, 1 };
    Sim_SetEntityDisplay(ent,
        { 1, 1, 0, 1 },
        { 1, 1, 0, 1 },
        COM_MESH_SPIKE_INDEX,
        SIM_DEATH_GFX_EXPLOSION);
    // must set birth tick here
    ent->timing.birthTick = def->birthTick;

    Sim_InitProjectile(
        ent,
        &def->pos,
        &def->velocity,
        &t,
        def->fastForwardTicks);

    return COM_ERROR_NONE;
}

internal SimEntity* Sim_SpawnEntity(
    SimScene* sim, SimEntSpawnData* def)
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
    
    COM_ASSERT(ent, "No free Entity")
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
        case SIM_FACTORY_TYPE_PROJ_PLAYER:
            err =  Sim_InitProjTest(sim, ent, def); break;
        case SIM_FACTORY_TYPE_EXPLOSION:
            err = Sim_InitExplosion(sim, ent, def); break;
        case SIM_FACTORY_TYPE_ACTOR:
            err =  Sim_InitActor(sim, ent, def); break;
        case SIM_FACTORY_TYPE_BOT:
            err =  Sim_InitBot(sim, ent, def); break;
        case SIM_FACTORY_TYPE_SEEKER:
            err =  Sim_InitSeeker(sim, ent, def); break;
        case SIM_FACTORY_TYPE_WANDERER:
            err =  Sim_InitWanderer(sim, ent, def); break;
        case SIM_FACTORY_TYPE_BOUNCER:
            err =  Sim_InitBouncer(sim, ent, def); break;
        case SIM_FACTORY_TYPE_DART:
            err = Sim_InitDart(sim, ent, def); break;
        case SIM_FACTORY_TYPE_RUBBLE:
            err = Sim_InitRubble(sim, ent, def); break;
        case SIM_FACTORY_TYPE_GRUNT:
            err = Sim_InitGrunt(sim, ent, def); break;
        case SIM_FACTORY_TYPE_WORLD:
            err =  Sim_InitWorldVolume(sim, ent, def); break;
        case SIM_FACTORY_TYPE_SPAWNER:
			err = Sim_InitSpawner(sim, ent, def); break;
        case SIM_FACTORY_TYPE_LINE_TRACE:
		    err =  Sim_InitLineTrace(sim, ent, def); break;
		case SIM_FACTORY_TYPE_NONE:
        {
            printf("SIM Cannot spawn, entity type not set!\n");
            
            return NULL;
        } break;

        default:
        {
            COM_ASSERT(0, "Sim Unknown entity type");
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
        #ifdef SIM_USE_PHYSICS_ENGINE
        if (ent->shape.handleId > 0)
        {
            PhysCmd_RemoveShape(sim->world, ent->shape.handleId);
            ent->shape = {};
        }
        #endif
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
