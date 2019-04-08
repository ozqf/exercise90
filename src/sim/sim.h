#pragma once

#include "../common/com_module.h"

#define SIM_ENT_STATUS_FREE 0
#define SIM_ENT_STATUS_RESERVED 1
#define SIM_ENT_STATUS_IN_USE 2

#define SIM_FACTORY_TYPE_NONE 0
#define SIM_FACTORY_TYPE_WORLD 1
#define SIM_FACTORY_TYPE_ACTOR 2
#define SIM_FACTORY_TYPE_PROJECTILE_BASE 3
#define SIM_FACTORY_TYPE_TURRET 4
#define SIM_FACTORY_TYPE_WANDERER 5
#define SIM_FACTORY_TYPE_LINE_TRACE 6
#define SIM_FACTORY_TYPE_PROJ_PREDICTION 7
#define SIM_FACTORY_TYPE_PROJ_TEST 8

#define SIM_TICK_TYPE_NONE 0
#define SIM_TICK_TYPE_WORLD 1
#define SIM_TICK_TYPE_ACTOR 2
#define SIM_TICK_TYPE_PROJECTILE 3
#define SIM_TICK_TYPE_TURRET 4
#define SIM_TICK_TYPE_WANDERER 5
#define SIM_TICK_TYPE_LINE_TRACE 6 

//#define SIM_PROJ_TYPE_NONE 0
//#define SIM_PROJ_TYPE_PLAYER_PREDICTION 1
//#define SIM_PROJ_TYPE_TEST 2

#define SIM_PATTERN_NONE 0
#define SIM_PATTERN_SPREAD 1
#define SIM_PATTERN_RADIAL 2

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
extern "C" i32      Sim_ReserveEntitySerial(SimScene* sim, i32 isLocal);
extern "C" i32      Sim_ReserveEntitySerials(
                        SimScene* scene, i32 isLocal, i32 count);
//extern "C" i32      Sim_ReserveSerialsForProjectiles(SimScene* sim, i32 isLocal, i32 projectileSpawnTime);

extern "C" i32      Sim_RestoreEntity(SimScene* sim, SimEntityDef* def);
extern "C" i32      Sim_RemoveEntity(SimScene* sim, i32 serialNumber);
extern "C" i32      Sim_SetActorInput(SimScene* sim, SimActorInput* input, i32 entitySerial);
//extern "C" i32      Sim_ExecuteProjectileSpawn(
//                        SimScene* sim,
//                        SimProjectileSpawnEvent* def,
//						i32 fastForwardTicks);
extern "C" i32      Sim_ExecuteEnemySpawn(
                        SimScene* sim,
                        SimEnemySpawnEvent* event,
	                    i32 fastForwardTicks);

// Entity behaviour
extern "C" void     Sim_SimpleMove(SimEntity* ent, f32 deltaTime);
extern "C" i32      Sim_InBounds(SimEntity* ent, Vec3* min, Vec3* max);
extern "C" void     Sim_BoundaryBounce(SimEntity* ent, Vec3* min, Vec3* max);

extern "C"
i32 Sim_FindByAABB(
    SimScene* sim,
    Vec3 boundsMin,
    Vec3 boundsMax,
    i32 ignoreSerial,
    SimEntity** results,
    i32 maxResults);
