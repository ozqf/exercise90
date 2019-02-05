#pragma once

#include "../common/com_module.h"

#include "sim_types.h"
#include "sim_command_types.h"

// Scene management
extern "C" void     Sim_InitScene(
                        SimScene* sim,
                        SimEntity* entityMemory,
                        i32 maxEntities);
extern "C" void 	Sim_Reset(SimScene* sim);
extern "C" i32 	    Sim_LoadScene(SimScene* sim, i32 index);
extern "C" i32      Sim_CalcEntityArrayBytes(i32 capacity);
extern "C" i32		Sim_GetFrameNumber(SimScene* sim);

// Entity list functions
extern "C" i32      Sim_ReserveEntitySerial(SimScene* sim, i32 isLocal);
extern "C" i32      Sim_ReserveEntitySerialGroup(SimScene* sim, i32 isLocal, i32 patternType);
extern "C" i32      Sim_AddEntity(SimScene* sim, SimEntityDef* def);
extern "C" i32      Sim_RemoveEntity(SimScene* sim, i32 serialNumber);
extern "C" i32      Sim_ExecuteProjectileSpawn(
                        SimScene* sim,
                        SimProjectileSpawnDef* def);

// Entity behaviour
extern "C" void     Sim_SimpleMove(SimEntity* ent, f32 deltaTime);
extern "C" i32      Sim_InBounds(SimEntity* ent, Vec3* min, Vec3* max);
extern "C" void     Sim_BoundaryBounce(SimEntity* ent, Vec3* min, Vec3* max);
