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
// Shared Entity initialisation
////////////////////////////////////////////////////////////////////
internal void Sim_InitEntity(SimEntity* ent, SimEntityDef* def)
{
    ent->birthTick =      def->birthTick;
    ent->body.t.pos =          def->pos;
    ent->body.previousPos =    def->pos;
    ent->destination =    def->destination;
    ent->body.velocity =       def->velocity;
    ent->relationships.childFactoryType = def->childFactoryType;
    ent->relationships.parentId.serial = def->parentSerial;

    Transform_SetScaleSafe(&ent->body.t, def->scale);
}

////////////////////////////////////////////////////////////////////
// Entity initialisation
////////////////////////////////////////////////////////////////////
internal i32 Sim_InitActor(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_InitEntity(ent, def);
    ent->tickType = SIM_TICK_TYPE_ACTOR;
    ent->attackTime = 0.2f;
    ent->display.colour = { 0, 1, 0, 1 };
    ent->flags = SIM_ENT_FLAG_POSITION_SYNC;
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
    ent->body.velocity = {};
    ent->display.colour = { 0.2f, 0.2f, 0.2f, 1 };
    return COM_ERROR_NONE;
}

internal i32 Sim_InitSpawner(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_InitEntity(ent, def);
    ent->tickType = SIM_TICK_TYPE_SPAWNER;
    ent->thinkTime = 4;//1.25f;
	ent->lifeTime = 10;
    ent->relationships.childSpawnCount = 4;
    ent->relationships.maxLiveChildren = 4;
    ent->relationships.totalChildren = 8;
    printf("Spawned Spawner, Ticktype %d Vel %.3f, %.3f, %.3f\n",
        ent->tickType, ent->body.velocity.x, ent->body.velocity.y, ent->body.velocity.z);
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

///////////////////////////////////////////////////////
// Enemies
///////////////////////////////////////////////////////
internal i32 Sim_InitWanderer(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_InitEntity(ent, def);
    ent->body.speed = 1;
    ent->tickType = SIM_TICK_TYPE_WANDERER;
    ent->display.colour = { 1, 0, 1, 1 };
    ent->flags = SIM_ENT_FLAG_SHOOTABLE
        | SIM_ENT_FLAG_POSITION_SYNC;
    return COM_ERROR_NONE;
}

internal i32 Sim_InitBouncer(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_InitEntity(ent, def);
    ent->body.speed = 4;
    ent->tickType = SIM_TICK_TYPE_BOUNCER;
    ent->display.colour = { 0.7f, 0.7f, 1, 1 };
    ent->flags = SIM_ENT_FLAG_SHOOTABLE
        | SIM_ENT_FLAG_POSITION_SYNC;
    return COM_ERROR_NONE;
}

internal i32 Sim_InitSeeker(
    SimScene* scene, SimEntity* ent, SimEntityDef* def)
{
    Sim_InitEntity(ent, def);
    ent->body.speed = 3;
    ent->tickType = SIM_TICK_TYPE_SEEKER;
    ent->display.colour = { 1, 0.2f, 1, 1 };
    ent->flags = SIM_ENT_FLAG_SHOOTABLE
        | SIM_ENT_FLAG_POSITION_SYNC;
    return COM_ERROR_NONE;
}

///////////////////////////////////////////////////////
// Projectiles
///////////////////////////////////////////////////////
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
    t.speed = 25.0f;
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
    t.speed = 25.0f;
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
        case SIM_FACTORY_TYPE_ACTOR:
            err =  Sim_InitActor(sim, ent, def); break;
        case SIM_FACTORY_TYPE_SEEKER:
            err =  Sim_InitSeeker(sim, ent, def); break;
        case SIM_FACTORY_TYPE_WANDERER:
            err =  Sim_InitWanderer(sim, ent, def); break;
        case SIM_FACTORY_TYPE_BOUNCER:
            err =  Sim_InitBouncer(sim, ent, def); break;
        case SIM_FACTORY_TYPE_WORLD:
            err =  Sim_InitWorldVolume(sim, ent, def); break;
        case SIM_TICK_TYPE_SPAWNER:
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
