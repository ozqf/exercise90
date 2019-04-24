#pragma once

#include "sim_internal.h"

internal void Sim_InitProjectile(
    SimEntity* ent,
    Vec3* pos,
    Vec3* velocity,
    SimProjectileType* type,
    i32 fastForwardTicks)
{
    ent->status = SIM_ENT_STATUS_IN_USE;
	ent->tickType = SIM_TICK_TYPE_PROJECTILE;
	ent->coreTickType = SIM_TICK_TYPE_PROJECTILE;
	Transform_SetToIdentity(&ent->body.t);
    if (!COM_IsZeroed((u8*)&type->scale, sizeof(Vec3)))
    {
        ent->body.t.scale = type->scale;
    }
	ent->body.t.pos = *pos;
    ent->body.velocity = *velocity;
    ent->body.speed = type->speed;

    ent->timing.nextThink = ent->timing.birthTick +
        App_CalcTickInterval(type->lifeTime);
	ent->timing.fastForwardTicks = fastForwardTicks;
}
