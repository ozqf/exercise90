#pragma once

#include "ZPhysics_internal_types.h"

internal PhysErrorHandler g_errorHandler = NULL;
internal ZBulletWorld g_world;
internal ByteBuffer g_input;
internal ByteBuffer g_output;
//internal PhysicsTestState g_physTest;

internal Vec3 g_testPos;

#define MAX_PHYS_BODIES 2048
#define MAX_PHYS_OVERLAPS 2048

internal PhysBodyHandle g_bodies[MAX_PHYS_BODIES];
internal PhysOverlapPair g_overlapPairs[MAX_PHYS_OVERLAPS];

internal const i32 g_debugStringSize = 1024;
internal char g_debugString[g_debugStringSize];
