#pragma once
/*
sim.h implementation
*/
#include "sim.h"

extern "C"
i32 Sim_CalcEntityArrayBytes(i32 capacity) { return (sizeof(SimEntity) * capacity); }

extern "C"
i32 Sim_GetFrameNumber(SimScene* sim){ return sim->tick; }

extern "C"
void Sim_SimpleMove(SimEntity* ent, f32 deltaTime)
{
    Vec3* pos = &ent->body.t.pos;
    ent->body.previousPos.x = pos->x;
    ent->body.previousPos.y = pos->y;
    ent->body.previousPos.z = pos->z;
    Vec3 move =
    {
        ent->body.velocity.x * deltaTime,
        ent->body.velocity.y * deltaTime,
        ent->body.velocity.z * deltaTime
    };
    
    ent->body.t.pos.x += move.x;
    ent->body.t.pos.y += move.y;
    ent->body.t.pos.z += move.z;
}

extern "C"
i32 Sim_InBounds(SimEntity* ent, Vec3* min, Vec3* max)
{
    Vec3* p = &ent->body.t.pos;
    if (p->x < min->x) { return 0; }
    if (p->x > max->x) { return 0; }
    if (p->y < min->y) { return 0; }
    if (p->y > max->y) { return 0; }
    if (p->z < min->z) { return 0; }
    if (p->z > max->z) { return 0; }
    return 1;
}

extern "C"
void Sim_BoundaryBounce(SimEntity* ent, Vec3* min, Vec3* max)
{
    Vec3* p = &ent->body.t.pos;
    if (p->x < min->x) { p->x = min->x; ent->body.velocity.x = -ent->body.velocity.x; }
    if (p->x > max->x) { p->x = max->x; ent->body.velocity.x = -ent->body.velocity.x; }

    if (p->y < min->y) { p->y = min->y; ent->body.velocity.y = -ent->body.velocity.y; }
    if (p->y > max->y) { p->y = max->y; ent->body.velocity.y = -ent->body.velocity.y; }

    if (p->z < min->z) { p->z = min->z; ent->body.velocity.z = -ent->body.velocity.z; }
    if (p->z > max->z) { p->z = max->z; ent->body.velocity.z = -ent->body.velocity.z; }
}

////////////////////////////////////////////////////////////
// Targetting and validation
////////////////////////////////////////////////////////////

// If no then this ent must be ignored by all other entities
extern "C"
i32 Sim_IsEntInPlay(SimEntity* ent)
{
    if (ent == NULL) { return NO; }
    if (ent->id.serial == SIM_ENT_NULL_SERIAL) { return NO; }
    if (ent->status != SIM_ENT_STATUS_IN_USE) { return NO; }
    if ((ent->flags & SIM_ENT_FLAG_OUT_OF_PLAY)) { return NO; }
    return YES;
}

// If no then this ent cannot be attacked
extern "C"
i32 Sim_IsEntTargetable(SimEntity* ent)
{
    if (Sim_IsEntInPlay(ent) == NO) { return NO; }
    if (!(ent->flags & SIM_ENT_FLAG_SHOOTABLE)) { return NO; }
    if ((ent->flags & SIM_ENT_FLAG_OUT_OF_PLAY)) { return NO; }
    return YES;
}

/**
 * Returns NULL if no suitable target can be found
 */
extern "C"
SimEntity* Sim_FindTargetForEnt(SimScene* sim, SimEntity* subject)
{
    for (i32 i = 0; i < sim->maxEnts; ++i)
    {
        SimEntity* ent = &sim->ents[i];
        if (Sim_IsEntInPlay(ent) == NO)
        { continue; }
        if (ent->factoryType != SIM_FACTORY_TYPE_ACTOR)
        { continue; }
        subject->relationships.targetId = ent->id;
        return ent;
    }
    subject->relationships.targetId = {};
    return NULL;
}

extern "C"
SimEntity* Sim_GetEntityBySerial(SimScene* sim, i32 serial)
{
    if (serial == SIM_ENT_NULL_SERIAL) { return NULL; }
    for (i32 i = 0; i < sim->maxEnts; ++i)
    {
        SimEntity* ent = &sim->ents[i];
        if (ent->status == SIM_ENT_STATUS_FREE) { continue; }
        if (ent->id.serial == serial) { return ent; }
    }
    return NULL;
}

extern "C"
SimEntity* Sim_GetEntityByIndex(SimScene* sim, SimEntIndex index)
{
    SimEntity* ent = &sim->ents[index.index];
    if (ent->id.slot.iteration == index.iteration
        && ent->id.serial != 0)
    {
        return ent;
    }
    return NULL;
}

extern "C"
i32 Sim_ReserveEntitySerial(SimScene* scene, i32 isLocal)
{
    if (isLocal) { return scene->localEntitySequence--; }
    else { return scene->remoteEntitySequence++; }
}

extern "C"
i32 Sim_ReserveEntitySerials(
    SimScene* scene, i32 isLocal, i32 count)
{
    i32 first;
    i32 last;
    if (isLocal)
    {
        first = scene->localEntitySequence;
        last = first - count;
        scene->localEntitySequence = last;
        APP_LOG(128,
            "SIM Reserving %d local entity serials (%d to %d)\n",
            count, first, (last - 1)
            );
    }
    else
    {
        first = scene->remoteEntitySequence;
        last = first + count;
        scene->remoteEntitySequence = last;
        APP_LOG(128,
            "SIM Reserving %d replicated entity serials (%d to %d)\n",
            count, first, (last - 1)
            );
    }
    return first;
}
/*
extern "C"
i32 Sim_ReserveSerialsForProjectiles(
    SimScene* sim, i32 isLocal, i32 projectileSpawnTime)
{
    i32 count = Sim_GetProjectileCount(projectileSpawnTime);
    return Sim_ReserveEntitySerials(sim, isLocal, count);
}
*/
/**
 * Restore the exact state of a single entity
 * (create it if it doesn't exist)
 */
extern "C"
SimEntity* Sim_RestoreEntity(SimScene* scene, SimEntSpawnData* def)
{
	// an id of zero is considered invalid
	COM_ASSERT(def->serial != SIM_ENT_NULL_SERIAL,
        "Restoring entity with null Serial");
    SimEntity* ent = Sim_GetEntityBySerial(scene, def->serial);
    // TODO: Handle this!
    COM_ASSERT(ent == NULL, "Entity already exists!")
    ent = Sim_SpawnEntity(scene, def);
    return ent;
}

extern "C"
i32 Sim_RemoveEntity(SimScene* scene, i32 serialNumber)
{
	COM_ASSERT(serialNumber != SIM_ENT_NULL_SERIAL,
        "Removing entity with null serial")
    return Sim_RecycleEntity(scene, serialNumber);
}

extern "C"
i32 Sim_SetActorInput(
    SimScene* sim,
    SimActorInput* input,
    i32 entitySerial)
{
    SimEntity* ent = Sim_FindEntityBySerialNumber(sim, entitySerial);
    if (!ent)
    {
        return COM_ERROR_NOT_FOUND;
    }
    ent->input = *input;
    return COM_ERROR_NONE;
}
#if 0
extern "C"
i32 Sim_ExecuteEnemySpawn(
    SimScene* sim,
    SimEnemySpawnEvent* event,
	i32 fastForwardTicks)
{
    i32 isLocal = (event->base.firstSerial < 0);

    SimSpawnPatternItem items[64];
    i32 count = Sim_CreateSpawnPattern(
        &event->base,
        &event->patternDef,
        items,
        event->base.firstSerial,
        isLocal);
    
    SimEntSpawnData def = {};
    def.factoryType = event->enemyType;

    for (i32 i = 0; i < event->patternDef.numItems; ++i)
    {
        SimSpawnPatternItem* item = &items[i];
        def.serial = item->entSerial;
        def.pos = items->pos;
        Sim_RestoreEntity(sim, &def);
    }
    
    return COM_ERROR_NONE;
}
#endif
#if 1
extern "C"
i32 Sim_ExecuteBulkSpawn(
    SimScene* sim,
    SimBulkSpawnEvent* event,
	i32 fastForwardTicks,
    i32* spawnedEntityFlags)
{
    //SimProjectileType* type = Sim_GetProjectileType(event->projType);
    COM_ASSERT(event->factoryType, "Bulk spawn factory type is 0")
    //Sim_SpawnProjectiles(sim, event, type, fastForwardTicks);
    i32 isLocal = (event->base.firstSerial < 0);
    
    SimSpawnPatternItem items[255];
    COM_ASSERT(event->patternDef.numItems < 256,
        "Pattern items > 255")
    i32 count = Sim_CreateSpawnPattern(
        &event->base,
        &event->patternDef,
        items,
        event->base.firstSerial,
        isLocal);
    COM_ASSERT(count > 0, "Pattern count underflow")
    for (i32 i = 0; i < count; ++i)
    {
        SimSpawnPatternItem* item = &items[i];
		//SimEntity* ent;
		//if (isLocal) { ent = Sim_GetFreeLocalEntity(sim, item->entSerial); }
		//else { ent = Sim_GetFreeReplicatedEntity(sim, item->entSerial); }
        
        SimEntSpawnData entDef = {};
        entDef.factoryType = event->factoryType;
        entDef.serial = item->entSerial;
        entDef.pos = item->pos;
        entDef.scale = { 1, 1, 1 };
        entDef.birthTick = sim->tick - fastForwardTicks;
        entDef.fastForwardTicks = fastForwardTicks;
        entDef.parentSerial = event->base.sourceSerial;
        SimEntity* ent = Sim_RestoreEntity(sim, &entDef);
        
        // TODO: Hack to find out whether we need to do priority queue syncing of this entity
        *spawnedEntityFlags = ent->flags;

        ent->body.velocity.x = item->forward.x * ent->body.speed;
        ent->body.velocity.y = item->forward.y * ent->body.speed;
        ent->body.velocity.z = item->forward.z * ent->body.speed;

        #if 0
        Vec3 v = {};
        v.x = item->forward.x * type->speed;
        v.y = item->forward.y * type->speed;
        v.z = item->forward.z * type->speed;
        
		Sim_InitProjectile(
            ent,
            &item->pos,
            &v,
            type,
            fastForwardTicks
        );
        #endif
    }

    return COM_ERROR_NONE;
}
#endif
extern "C"
void Sim_Reset(SimScene* sim)
{
	i32 arraySize = Sim_CalcEntityArrayBytes(sim->maxEnts);
	i32 numBytes = sim->maxEnts * sizeof(SimEntity);
	COM_ZeroMemory((u8*)sim->ents, arraySize);
	sim->cmdSequence = 0;
	sim->tick = 0;
	// 0 == an invalid serial for error handling. Means once less
	// replicated entity, oh well
	sim->remoteEntitySequence = 1;
	sim->localEntitySequence = -1;
    //Sim_InitProjectileTypes();
}

extern "C"
void Sim_Init(
            SimScene* sim,
            SimEntity* entityMemory,
            i32 maxEntities)
{
    *sim = {};
    
    sim->ents = entityMemory;
    sim->maxEnts = maxEntities;
	Sim_Reset(sim);
}

extern "C"
i32 Sim_LoadScene(SimScene* sim, i32 index)
{
    f32 halfX = 30;
    f32 halfY = 20;
    f32 halfZ = 20;

    SimEntSpawnData def = {};
    def.serial = Sim_ReserveEntitySerial(sim, 1);
    def.isLocal = 1;
	def.factoryType = SIM_FACTORY_TYPE_WORLD;
    def.pos.y = -0.5f;
    def.scale.x = halfX * 2;
    def.scale.y = 1;
    def.scale.z = halfZ * 2;

    Sim_RestoreEntity(sim, &def);

    halfX -= 1;
    halfY -= 1;
    halfZ -= 1;

    sim->boundaryMin = { -halfX, -halfY, -halfZ };
    sim->boundaryMax = { halfX, halfY, halfZ };

	return COM_ERROR_NONE;
}
