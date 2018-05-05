#pragma once

#include "ZPhysics_internal_types.h"

PhysBodyHandle* PHYS_GetFreeBodyHandle(PhysBodyList* list);

void Phys_FreeHandle(ZBulletWorld* world, PhysBodyHandle* handle);

void Phys_RecycleHandle(ZBulletWorld* world, PhysBodyHandle* handle);

PhysBodyHandle* Phys_CreateBulletSphere(ZBulletWorld* world, ZSphereDef def);

PhysBodyHandle* Phys_CreateBulletBox(ZBulletWorld* world, ZBoxDef def);

internal void Phys_PreSolveCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);

internal void Phys_PostSolveCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);

internal void Phys_WriteDebugOutput(ZBulletWorld* world);
