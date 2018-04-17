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

int Phys_CreateShape();

int Phys_RemoveShape();

int Phys_QueryHitscan();

int Phys_QueryVolume();

Vec3 Phys_DebugGetPosition();

void Phys_Init();

void Phys_Step(f32 deltaTime);

void Phys_Shutdown();
