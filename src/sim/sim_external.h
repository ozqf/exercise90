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
}

extern "C"
i32 Sim_InBounds(SimEntity* ent, Vec3* min, Vec3* max)
{
    Vec3* p = &ent->t.pos;
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
    Vec3* p = &ent->t.pos;
    if (p->x < min->x) { p->x = min->x; ent->velocity.x = -ent->velocity.x; }
    if (p->x > max->x) { p->x = max->x; ent->velocity.x = -ent->velocity.x; }

    if (p->y < min->y) { p->y = min->y; ent->velocity.y = -ent->velocity.y; }
    if (p->y > max->y) { p->y = max->y; ent->velocity.y = -ent->velocity.y; }

    if (p->z < min->z) { p->z = min->z; ent->velocity.z = -ent->velocity.z; }
    if (p->z > max->z) { p->z = max->z; ent->velocity.z = -ent->velocity.z; }
}

extern "C"
SimEntity* Sim_GetEntityBySerial(SimScene* sim, i32 serial)
{
    if (serial == 0) { return NULL; }
    for (i32 i = 0; i < sim->maxEnts; ++i)
    {
        SimEntity* ent = &sim->ents[i];
        if (ent->status == SIM_ENT_STATUS_FREE) { continue; }
        if (ent->id.serial == serial) { return ent; }
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
i32 Sim_RestoreEntity(SimScene* scene, SimEntityDef* def)
{
	// an id of zero is considered invalid
	Assert(def->serial)
    SimEntity* ent = Sim_GetEntityBySerial(scene, def->serial);
    if (ent)
    {
        // TODO: This code!
        ILLEGAL_CODE_PATH
        return COM_ERROR_NOT_IMPLEMENTED;
    }
    else
    {
        return Sim_SpawnEntity(scene, def);
    }
}

extern "C"
i32 Sim_RemoveEntity(SimScene* scene, i32 serialNumber)
{
	Assert(serialNumber)
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
    
    SimEntityDef def = {};
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
i32 Sim_ExecuteProjectileSpawn(
    SimScene* sim,
    SimProjectileSpawnEvent* event,
	i32 fastForwardTicks)
{
    //SimProjectileType* type = Sim_GetProjectileType(event->projType);
    Assert(event->factoryType)
    //Sim_SpawnProjectiles(sim, event, type, fastForwardTicks);
    i32 isLocal = (event->base.firstSerial < 0);

    SimSpawnPatternItem items[64];
    i32 count = Sim_CreateSpawnPattern(
        &event->base, &event->patternDef, items, event->base.firstSerial, isLocal);
    
    for (i32 i = 0; i < count; ++i)
    {
        SimSpawnPatternItem* item = &items[i];
		//SimEntity* ent;
		//if (isLocal) { ent = Sim_GetFreeLocalEntity(sim, item->entSerial); }
		//else { ent = Sim_GetFreeReplicatedEntity(sim, item->entSerial); }
        
        f32 speed = 15.0f;
        SimEntityDef entDef = {};
        entDef.factoryType = event->factoryType;
        entDef.serial = item->entSerial;
        entDef.pos = item->pos;
        entDef.scale = { 1, 1, 1 };
        entDef.velocity.x = item->forward.x * speed;
        entDef.velocity.y = item->forward.y * speed;
        entDef.velocity.z = item->forward.z * speed;
        entDef.fastForwardTicks = fastForwardTicks;
        Sim_RestoreEntity(sim, &entDef);

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
    f32 halfX = 10;
    f32 halfY = 10;
    f32 halfZ = 10;

    SimEntityDef def = {};
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
