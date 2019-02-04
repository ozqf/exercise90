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
i32 Sim_ReserveEntitySerial(SimScene* scene, i32 isLocal)
{
    if (isLocal) { return scene->localEntitySequence++; }
    else { return scene->remoteEntitySequence++; }
}

extern "C"
i32 Sim_ReserveEntitySerialGroup(SimScene* scene, i32 isLocal, i32 patternType)
{
    switch (patternType)
    {
        case SIM_PROJ_TYPE_TEST:
        {
            i32 serial;
            if (isLocal)
            {
                serial = scene->localEntitySequence;
                scene->localEntitySequence += 8;
            }
            else
            {
                serial = scene->remoteEntitySequence;
                scene->remoteEntitySequence += 8;
            }
            return serial;
        }
    }
    return -1;
}

extern "C"
i32 Sim_AddEntity(SimScene* scene, SimEntityDef* def)
{
    return Sim_SpawnEntity(scene, def);
}

extern "C"
i32 Sim_RemoveEntity(SimScene* scene, i32 serialNumber)
{
    return Sim_RecycleEntity(scene, serialNumber);
}

extern "C"
i32 Sim_ExecuteProjectileSpawn(
    SimScene* sim,
    SimProjectileSpawnDef* def)
{
    return COM_ERROR_NOT_IMPLEMENTED;
}

extern "C"
void Sim_InitScene(
            SimScene* scene,
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
}

extern "C"
i32 Sim_LoadScene(SimScene* sim, i32 index)
{
    SimEntityDef def = {};
    def.isLocal = 1;
	def.entType = SIM_ENT_TYPE_WORLD;
    def.pos[1] = 0;
    def.scale[0] = 12;
    def.scale[1] = 1;
    def.scale[2] = 12;
    Sim_AddEntity(sim, &def);

    sim->boundaryMin = { -6, -6, -6 };
    sim->boundaryMax = { 6, 6, 6 };

	return COM_ERROR_NONE;
}
