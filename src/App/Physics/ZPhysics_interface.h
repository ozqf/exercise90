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

void Phys_GetDebugString(char** str, i32* length);

i32 Phys_CreateShape(ZShapeDef* def, u16 ownerId, u16 ownerIteration);

i32 Phys_CreateBox(
    f32 x,
    f32 y,
    f32 z,
    f32 halfSizeX,
    f32 halfSizeY,
    f32 halfSizeZ,
    u32 flags,
    u16 group,
    u16 mask,
    u16 ownerId,
    u16 ownerIteration
    );

//i32 Phys_CreateSphere(f32 x, f32 y, f32 z, f32 radius, u32 flags, u16 mask, u16 ownerId, u16 ownerIteration);

//int Phys_CreateBox(f32 x, f32 y, f32 z, f32 halfWidthX, f32 halfWidthY, f32 halfWidthZ, u32 flags, u16 mask, u16 ownerId, u16 ownerIteration);

//i32 Phys_CreateInfinitePlane(f32 y, u16 mask, u16 ownerId, u16 ownerIteration);

i32 Phys_RemoveShape();

void Phys_TeleportShape(i32 shapeId, f32 posX, f32 posY, f32 posZ);

i32 Phys_QueryHitscan();

i32 Phys_QueryVolume();

Vec3 Phys_DebugGetPosition();

void Phys_Init(void* ptrCommandBuffer, u32 commandBufferSize);

//void Phys_ReadCommands(MemoryBlock* commandBuffer);

void Phys_Step(MemoryBlock* eventBuffer, f32 deltaTime);

void Phys_Shutdown();
