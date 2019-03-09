#pragma once

#include "sim_internal.h"

#define SIM_MAX_PROJECTILE_TYPES 256

internal SimProjectileType g_prjTypes[SIM_MAX_PROJECTILE_TYPES];

internal void Sim_InitProjectiles()
{
    i32 bytes = sizeof(SimProjectileType) * SIM_MAX_PROJECTILE_TYPES;
    COM_ZeroMemory((u8*)g_prjTypes, bytes);
    SimProjectileType* t;

    t = &g_prjTypes[SIM_PROJ_TYPE_NONE];
    t->speed = 15.0f;
    t->numProjectiles = 1;
    t->lifeTime = 2.0f;
    t->pattern = SIM_PROJECTILE_PATTERN_NONE;

    t = &g_prjTypes[SIM_PROJ_TYPE_TEST];
    t->speed = 3.5f;
    t->numProjectiles = 4;
    t->lifeTime = 6.0f;
    t->pattern = SIM_PROJECTILE_PATTERN_RADIAL;
}

internal SimProjectileType* Sim_GetProjectileType(i32 index)
{
    if (index < 0) { index = 0; }
    if (index >= SIM_MAX_PROJECTILE_TYPES) { index = 0; }
    return &g_prjTypes[index];
}

internal i32 Sim_GetProjectileCount(i32 index)
{
    if (index < 0 || index >= SIM_MAX_PROJECTILE_TYPES)
    {
        return 0;
    }
    return (Sim_GetProjectileType(index))->numProjectiles;
}

internal void Sim_InitProjectile(
    SimEntity* ent,
    Vec3* pos,
    Vec3* velocity,
    f32 lifeTime,
    i32 fastForwardTicks)
{
    ent->status = SIM_ENT_STATUS_IN_USE;
	ent->entType = SIM_ENT_TYPE_PROJECTILE;
	Transform_SetToIdentity(&ent->t);
    ent->lifeTime = lifeTime;
	ent->t.pos = *pos;
    ent->velocity = *velocity;
	ent->fastForwardTicks = fastForwardTicks;
}

internal void Sim_NullProjectilePattern(
    SimScene* sim,
    SimProjectileSpawnDef* event,
    SimProjectileType* type,
    i32 fastForwardTicks)
{
    i32 serial = event->firstSerial;
    f32 radians = atan2f(event->forward.z, event->forward.x);
    for (i32 i = 0; i < type->numProjectiles; ++i)
    {
        Vec3 v = {};
        v.x = cosf(radians) * type->speed;
	    v.y = 0;
	    v.z = sinf(radians) * type->speed;

        SimEntity* ent = Sim_GetFreeReplicatedEntity(sim, serial);
		Sim_InitProjectile(
            ent,
            &event->pos,
            &v,
            type->lifeTime,
            fastForwardTicks
        );
		APP_LOG(256, "SIM prj %d: pos %.3f, %.3f, %.3f. Fast-forward: %d\n",
            serial,
            ent->t.pos.x, ent->t.pos.y, ent->t.pos.z,
            ent->fastForwardTicks
		);
        serial++;
    }
}

internal void Sim_RadialProjectilePattern(
    SimScene* sim,
    SimProjectileSpawnDef* event,
    SimProjectileType* type,
    i32 fastForwardTicks)
{
    i32 serial = event->firstSerial;
	// increment is positive if remote, negative if local
	i32 isLocal = (event->firstSerial < 0);
	i32 increment = isLocal ? -1 : 1;
	f32 radians = 0;
	f32 step = FULL_ROTATION_RADIANS / (f32)type->numProjectiles;
	
    for (i32 i = 0; i < type->numProjectiles; ++i)
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
            &event->pos,
            &v,
            type->lifeTime,
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
    SimProjectileSpawnDef* event,
    SimProjectileType* type,
    i32 fastForwardTicks)
{

}
