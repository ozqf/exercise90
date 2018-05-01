#pragma once

/**
 * Interface to physics engine
 * 
 * Phys_Bullet
 * 
 */
#include "../../common/com_module.h"

#include "ZPhysics_external_types.h"

//////////////////////////////////////////////////////////////////
// Collision API
//////////////////////////////////////////////////////////////////

i32 Phys_CreateSphere(f32 x, f32 y, f32 z, f32 radius, u16 ownerId, u16 ownerIteration);

i32 Phys_RemoveShape();

i32 Phys_QueryHitscan();

i32 Phys_QueryVolume();

Vec3 Phys_DebugGetPosition();

void Phys_Init();

void Phys_Step(MemoryBlock* eventBuffer, f32 deltaTime);

void Phys_Shutdown();
