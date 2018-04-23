#pragma once

/**
 * Interface to physics engine
 * 
 * Phys_Bullet
 * 
 */
#include "../../Shared/shared_types.h"

#include "ZPhysics_external_types.h"

//////////////////////////////////////////////////////////////////
// Collision API
//////////////////////////////////////////////////////////////////

i32 Phys_CreateSphere(f32 x, f32 y, f32 z, f32 radius);

i32 Phys_RemoveShape();

i32 Phys_QueryHitscan();

i32 Phys_QueryVolume();

Vec3 Phys_DebugGetPosition();

void Phys_Init();

void Phys_Step(f32 deltaTime);

void Phys_Shutdown();
