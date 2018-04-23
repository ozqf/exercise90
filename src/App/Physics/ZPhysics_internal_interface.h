#pragma once

#include "ZPhysics_internal_types.h"

PhysBodyHandle* PHYS_GetFreeBodyHandle(PhysBodyList* list);

void Phys_FreeHandle(ZBulletWorld* world, PhysBodyHandle* handle);

void Phys_RecycleHandle(ZBulletWorld* world, PhysBodyHandle* handle);

PhysBodyHandle* Phys_CreateBulletSphere(ZBulletWorld* world, f32 x, f32 y, f32 z, f32 radius);


