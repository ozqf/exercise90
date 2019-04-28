#pragma once

#include "../common/common.h"

#define SIM_NET_MIN_PRIORITY 1
#define SIM_NET_MAX_PRIORITY 3

#define SIM_DEFAULT_SPAWN_DELAY 1.5

#define SIM_ENT_STATUS_FREE 0
#define SIM_ENT_STATUS_RESERVED 1
#define SIM_ENT_STATUS_IN_USE 2

#define SIM_ENT_NULL_SERIAL 0

// Spawn functions
typedef u8 simFactoryType;

#define SIM_FACTORY_TYPE_NONE 0
#define SIM_FACTORY_TYPE_WORLD 1
#define SIM_FACTORY_TYPE_ACTOR 2
#define SIM_FACTORY_TYPE_PROJECTILE_BASE 3
#define SIM_FACTORY_TYPE_SPAWNER 4
#define SIM_FACTORY_TYPE_WANDERER 5
#define SIM_FACTORY_TYPE_LINE_TRACE 6
#define SIM_FACTORY_TYPE_PROJ_PREDICTION 7
#define SIM_FACTORY_TYPE_PROJ_PLAYER 8
#define SIM_FACTORY_TYPE_BOUNCER 9
#define SIM_FACTORY_TYPE_SEEKER 10
#define SIM_FACTORY_TYPE_RUBBLE 11
#define SIM_FACTORY_TYPE_DART 12
#define SIM_FACTORY_TYPE_EXPLOSION 13

// Update functions
#define SIM_TICK_TYPE_NONE 0
#define SIM_TICK_TYPE_WORLD 1
#define SIM_TICK_TYPE_ACTOR 2
#define SIM_TICK_TYPE_PROJECTILE 3
#define SIM_TICK_TYPE_SPAWNER 4
#define SIM_TICK_TYPE_WANDERER 5
#define SIM_TICK_TYPE_LINE_TRACE 6
#define SIM_TICK_TYPE_BOUNCER 7
#define SIM_TICK_TYPE_SEEKER 9
#define SIM_TICK_TYPE_DART 10
#define SIM_TICK_TYPE_EXPLOSION 11
#define SIM_TICK_TYPE_SPAWN 12

// Spawn pattern types.
#define SIM_PATTERN_NONE 0
#define SIM_PATTERN_SPREAD 1
#define SIM_PATTERN_RADIAL 2
#define SIM_PATTERN_SCATTER 3
#define SIM_PATTERN_LINE 4
#define SIM_PATTERN_CIRCLE 5

// Sim Entity flags
#define SIM_ENT_FLAG_OUT_OF_PLAY (1 << 0)
#define SIM_ENT_FLAG_SHOOTABLE (1 << 1)
#define SIM_ENT_FLAG_POSITION_SYNC (1 << 2)
#define SIM_ENT_FLAG_MOVE_AVOID (1 << 3)
#define SIM_ENT_FLAG_TARGET_SEEKING (1 << 4)

#define SIM_DEATH_GFX_NONE 0
#define SIM_DEATH_GFX_EXPLOSION 1

#include "sim_types.h"

// Scene management
extern "C" void     Sim_Init(
                        SimScene* sim,
                        SimEntity* entityMemory,
                        i32 maxEntities);
extern "C" void 	Sim_Reset(SimScene* sim);
extern "C" i32 	    Sim_LoadScene(SimScene* sim, i32 index);
extern "C" i32      Sim_CalcEntityArrayBytes(i32 capacity);
extern "C" i32		Sim_GetFrameNumber(SimScene* sim);

// Entity list functions
extern "C" SimEntity* Sim_GetEntityBySerial(SimScene* sim, i32 serial);
extern "C" SimEntity* Sim_GetEntityByIndex(SimScene* sim, SimEntIndex index);
extern "C" i32      Sim_ReserveEntitySerial(SimScene* sim, i32 isLocal);
extern "C" i32      Sim_ReserveEntitySerials(
                        SimScene* scene, i32 isLocal, i32 count);
extern "C" SimEntity* Sim_RestoreEntity(SimScene* sim, SimEntSpawnData* def);
extern "C" i32      Sim_RemoveEntity(SimScene* sim, i32 serialNumber);
extern "C" i32      Sim_SetActorInput(
    SimScene* sim, SimActorInput* input, i32 entitySerial);
extern "C" i32      Sim_ExecuteBulkSpawn(
                        SimScene* sim,
                        SimBulkSpawnEvent* def,
						i32 fastForwardTicks,
                        i32* spawnedEntityFlags,
                        f32* spawnedEntityPriority);

// Entity behaviour
extern "C" void     Sim_SimpleMove(SimEntity* ent, f32 deltaTime);
extern "C" i32      Sim_InBounds(SimEntity* ent, Vec3* min, Vec3* max);
extern "C" void     Sim_BoundaryBounce(SimEntity* ent, Vec3* min, Vec3* max);

extern "C" i32      Sim_TickSpawner(
    SimScene* sim, SimEntity* ent, f32 deltaTime);

// Searching/Querying
extern "C" i32        Sim_IsEntInPlay(SimEntity* ent);
extern "C" SimEntity* Sim_FindTargetForEnt(SimScene* sim, SimEntity* subject);
extern "C" i32        Sim_IsEntTargetable(SimEntity* ent);

extern "C"
i32 Sim_FindByAABB(
    SimScene* sim,
    Vec3 boundsMin,
    Vec3 boundsMax,
    i32 ignoreSerial,
    SimEntity** results,
    i32 maxResults);

extern "C"
Vec3 Sim_BuildAvoidVector(
    SimScene* sim,
    SimEntity* mover);
