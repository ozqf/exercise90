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
i32 Sim_ReserveEntitySerialGroup(SimScene* scene, i32 isLocal, i32 patternType)
{
    switch (patternType)
    {
        case SIM_PROJ_TYPE_TEST:
        {
            i32 first;
            i32 last;
            i32 count = 8;
            if (isLocal)
            {
                first = scene->localEntitySequence;
                last = first + count;
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
        default:
        {
            APP_LOG(128, "SIM Cannot reserve serials for pattern type %d\n", patternType);
        } break;
    }
    return COM_ERROR_BAD_INDEX;
}

extern "C"
i32 Sim_AddEntity(SimScene* scene, SimEntityDef* def)
{
	// an id of zero is considered invalid
	Assert(def->serial)
    return Sim_SpawnEntity(scene, def);
}

extern "C"
i32 Sim_RemoveEntity(SimScene* scene, i32 serialNumber)
{
	Assert(serialNumber)
    return Sim_RecycleEntity(scene, serialNumber);
}

extern "C"
i32 Sim_ExecuteProjectileSpawn(
    SimScene* sim,
    SimProjectileSpawnDef* def,
	i32 fastForwardTicks)
{
    i32 numProjectiles = 8;
	f32 speed = 3.5f;
    i32 serial = def->firstSerial;
	f32 radians = 0;
	f32 step = FULL_ROTATION_RADIANS / (f32)numProjectiles;
    for (i32 i = 0; i < numProjectiles; ++i)
    {
        SimEntity* ent = Sim_GetFreeReplicatedEntity(sim, serial);
		ent->status = SIM_ENT_STATUS_IN_USE;
		ent->entType = SIM_ENT_TYPE_PROJECTILE;
		Transform_SetToIdentity(&ent->t);
        ent->lifeTime = 6.0f;
		ent->t.pos = def->pos;
		ent->velocity.x = cosf(radians) * speed;
		ent->velocity.y = 0;
		ent->velocity.z = sinf(radians) * speed;
        // if fastForwardTicks
		ent->fastForwardTicks = fastForwardTicks;
		APP_LOG(256, "SIM prj %d: pos %.3f, %.3f, %.3f. Fast-forward: %d\n",
            serial,
            ent->t.pos.x, ent->t.pos.y, ent->t.pos.z,
            ent->fastForwardTicks
		);
        serial++;
		radians += step;
    }
    return COM_ERROR_NONE;
}

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
}

extern "C"
void Sim_InitScene(
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
    SimEntityDef def = {};
    def.serial = Sim_ReserveEntitySerial(sim, 1);
    def.isLocal = 1;
	def.entType = SIM_ENT_TYPE_WORLD;
    def.pos[1] = -0.5f;
    def.scale[0] = 20;
    def.scale[1] = 1;
    def.scale[2] = 20;
    
    Sim_AddEntity(sim, &def);

    sim->boundaryMin = { -6, -6, -6 };
    sim->boundaryMax = { 6, 6, 6 };

	return COM_ERROR_NONE;
}
