#pragma once

// TODO: Remove: This include is purely due to requiring access to APP_LOG and APP_PRINT!
#include "../app/app.h"
#include "sim.h"
#include <math.h>
// Internal Sim module functions/data

#define SIM_PROJECTILE_PATTERN_NONE 0
#define SIM_PROJECTILE_PATTERN_SPREAD 1
#define SIM_PROJECTILE_PATTERN_RADIAL 2

struct SimProjectileType
{
    f32 speed;
    i32 numProjectiles;
    i32 pattern;
    f32 lifeTime;
    f32 arcDegrees;
};

internal SimProjectileType* Sim_GetProjectileType(i32 index);
internal SimEntity* Sim_FindEntityBySerialNumber(SimScene* scene, i32 serialNumber);
internal SimEntity* Sim_GetFreeReplicatedEntity(SimScene* scene, i32 newSerial);
internal SimEntity* Sim_GetFreeLocalEntity(SimScene* scene, i32 newSerial);
internal i32        Sim_FreeEntityBySerial(SimScene* scene, i32 serial);
internal i32        Sim_FindFreeSlot(SimScene* scene, i32 forLocalEnt);
internal i32        Sim_EnqueueCommand(SimScene* sim, u8* ptr);
internal i32        Sim_RecycleEntity(SimScene* sim, i32 entitySerialNumber);
