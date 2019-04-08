#pragma once

#include "sim_internal.h"

//#define SIM_MAX_PROJECTILE_TYPES 256

//internal SimProjectileType g_prjTypes[SIM_MAX_PROJECTILE_TYPES];

internal void Sim_InitProjectile(
    SimEntity* ent,
    Vec3* pos,
    Vec3* velocity,
    SimProjectileType* type,
    i32 fastForwardTicks)
{
    ent->status = SIM_ENT_STATUS_IN_USE;
	ent->tickType = SIM_TICK_TYPE_PROJECTILE;
	Transform_SetToIdentity(&ent->t);
    if (!COM_IsZeroed((u8*)&type->scale, sizeof(Vec3)))
    {
        ent->t.scale = type->scale;
    }
	ent->t.pos = *pos;
    ent->velocity = *velocity;

    ent->lifeTime = type->lifeTime;
	ent->fastForwardTicks = fastForwardTicks;
}
#if 0
/*
Define all entity types here
*/
internal void Sim_InitProjectileTypes()
{
    i32 bytes = sizeof(SimProjectileType) * SIM_MAX_PROJECTILE_TYPES;
    COM_ZeroMemory((u8*)g_prjTypes, bytes);
    SimProjectileType* t;

    t = &g_prjTypes[SIM_PROJ_TYPE_NONE];
    t->speed = 10.0f;
    t->patternDef.numItems = 1;
    t->lifeTime = 2.0f;
    t->patternDef.patternId = SIM_PATTERN_NONE;

    t = &g_prjTypes[SIM_PROJ_TYPE_PLAYER_PREDICTION];
    t->speed = 10.0f;
    t->patternDef.numItems = 1;
    t->lifeTime = 2.0f;
    t->patternDef.patternId = SIM_PATTERN_NONE;
    t->scale = { 0.5f, 0.5f, 0.5f };

    t = &g_prjTypes[SIM_PROJ_TYPE_TEST];
    t->speed = 10.0f;
    t->patternDef.numItems = 4;
    t->lifeTime = 6.0f;
    t->patternDef.patternId = SIM_PATTERN_RADIAL;
}

internal SimProjectileType* Sim_GetProjectileType(i32 index)
{
    if (index < 0) { index = 0; }
    if (index >= SIM_MAX_PROJECTILE_TYPES) { index = 0; }
    return &g_prjTypes[index];
}

/*
Necessary to reserve blocks of contiguous entity serial numbers when
spawning deterministic groups
*/
internal i32 Sim_GetProjectileCount(i32 index)
{
    if (index < 0 || index >= SIM_MAX_PROJECTILE_TYPES)
    {
        return 0;
    }
    return (Sim_GetProjectileType(index))->patternDef.numItems;
}

internal void Sim_SpawnProjectiles(
    SimScene* sim,
    SimProjectileSpawnEvent* event,
    SimProjectileType* type,
    i32 fastForwardTicks)
{
    i32 isLocal = (event->base.firstSerial < 0);

    SimSpawnPatternItem items[64];
    i32 count = Sim_CreateSpawnPattern(
        &event->base, &type->patternDef, items, event->base.firstSerial, isLocal);
    
    for (i32 i = 0; i < count; ++i)
    {
        SimSpawnPatternItem* item = &items[i];
		SimEntity* ent;
		if (isLocal) { ent = Sim_GetFreeLocalEntity(sim, item->entSerial); }
		else { ent = Sim_GetFreeReplicatedEntity(sim, item->entSerial); }

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
    }
}
#endif
#if 0
internal void Sim_NullProjectilePattern(
    SimScene* sim,
    SimProjectileSpawnEvent* event,
    SimProjectileType* type,
    i32 fastForwardTicks)
{
    i32 serial = event->base.firstSerial;
    f32 radians = atan2f(event->base.forward.z, event->base.forward.x);
    for (i32 i = 0; i < type->patternDef.numItems; ++i)
    {
        Vec3 v = {};
        v.x = cosf(radians) * type->speed;
	    v.y = 0;
	    v.z = sinf(radians) * type->speed;

        SimEntity* ent = Sim_GetFreeReplicatedEntity(sim, serial);
		Sim_InitProjectile(
            ent,
            &event->base.pos,
            &v,
            type,
            fastForwardTicks
        );
		APP_LOG(256,
            "SIM prj %d: pos %.3f, %.3f, %.3f. Fast-forward: %d\n",
            serial,
            ent->t.pos.x, ent->t.pos.y, ent->t.pos.z,
            ent->fastForwardTicks
		);
        serial++;
    }
}

internal void Sim_RadialProjectilePattern(
    SimScene* sim,
    SimProjectileSpawnEvent* event,
    SimProjectileType* type,
    i32 fastForwardTicks)
{
    i32 serial = event->base.firstSerial;
	// increment is positive if remote, negative if local
	i32 isLocal = (event->base.firstSerial < 0);
	i32 increment = isLocal ? -1 : 1;
	f32 radians = 0;
	f32 step = FULL_ROTATION_RADIANS / (f32)type->patternDef.numItems;
	
    for (i32 i = 0; i < type->patternDef.numItems; ++i)
    {
        Vec3 v = {};
        v.x = cosf(radians) * type->speed;
	    v.y = 0;
	    v.z = sinf(radians) * type->speed;
		
		
		SimEntity* ent;
		if (isLocal) { ent = Sim_GetFreeLocalEntity(sim, serial); }
		else { ent = Sim_GetFreeReplicatedEntity(sim, serial); }
		
		Sim_InitProjectile(
            ent,
            &event->base.pos,
            &v,
            type,
            fastForwardTicks
        );
		APP_LOG(256, "SIM prj %d: pos %.3f, %.3f, %.3f. Fast-forward: %d\n",
            serial,
            ent->t.pos.x, ent->t.pos.y, ent->t.pos.z,
            ent->fastForwardTicks
		);
        serial += increment;
		radians += step;
    }
}

internal void Sim_SpreadProjectilePattern(
    SimScene* sim,
    SimProjectileSpawnEvent* event,
    SimProjectileType* type,
    i32 fastForwardTicks)
{

}
#endif