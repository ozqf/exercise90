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

void Phys_SetState(PhysCmd_State* state);

void Phys_TeleportShape(i32 shapeId, f32 posX, f32 posY, f32 posZ);

void Phys_ChangeVelocity(i32 shapeId, f32 velX, f32 velY, f32 velZ);

i32 Phys_RayTest(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 y2);

i32 Phys_QueryRay(PhysCmd_Raycast* cmd, PhysRayHit* hits, i32 maxHits);

//i32 Phys_QueryRay(PhysCmd_Raycast* cmd, u8* resultsBuffer, u32 bufferCapacity);

i32 Phys_QueryHitscan();

i32 Phys_QueryVolume();

Vec3 Phys_DebugGetPosition();

// Must pass two blocks of memory for the physics to use for I/O
// Size... to be determined
void Phys_Init(void* ptrCommandBuffer, u32 commandBufferSize, void* ptrEventBuffer, u32 eventBufferSize);

//void Phys_ReadCommands(MemoryBlock* commandBuffer);

// Returns memory block of the event buffer assigned at init
MemoryBlock Phys_Step(f32 deltaTime);

// Remove all objects from simulation, resetting it
void Phys_ClearWorld();

// Shut down library. destroy all objects.
void Phys_Shutdown();
