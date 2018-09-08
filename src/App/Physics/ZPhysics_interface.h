#pragma once

/**
 * Collision API
 * Interface to physics engine
 * > PhysCmd == enqueue commands that are executed when the next step is called
 * > PhysExt == unqueued, external and executed immediately
 * 
 * Phys_Bullet
 * 
 */
#include "../../common/com_module.h"

#include "ZPhysics_external_types.h"

//////////////////////////////////////////////////////////////////
// Queued commands
//////////////////////////////////////////////////////////////////
i32 PhysCmd_CreateShape(ZShapeDef* def, u32 ownerId);
i32 PhysCmd_CreateBox(
    f32 x,
    f32 y,
    f32 z,
    f32 halfSizeX,
    f32 halfSizeY,
    f32 halfSizeZ,
    u32 flags,
    u16 group,
    u16 mask,
    u32 ownerId
    );
//i32 Phys_CreateSphere(f32 x, f32 y, f32 z, f32 radius, u32 flags, u16 mask, u16 ownerId, u16 ownerIteration);
//int Phys_CreateBox(f32 x, f32 y, f32 z, f32 halfWidthX, f32 halfWidthY, f32 halfWidthZ, u32 flags, u16 mask, u16 ownerId, u16 ownerIteration);
//i32 Phys_CreateInfinitePlane(f32 y, u16 mask, u16 ownerId, u16 ownerIteration);
void PhysCmd_RemoveShape(i32 shapeId);
void PhysCmd_SetState(PhysCmd_State* state);
void PhysCmd_TeleportShape(i32 shapeId, f32 posX, f32 posY, f32 posZ);
void PhysCmd_ChangeVelocity(i32 shapeId, f32 velX, f32 velY, f32 velZ);

//////////////////////////////////////////////////////////////////
// Instant commands
//////////////////////////////////////////////////////////////////
void PhysExt_GetDebugString(char** str, i32* length);
i32 PhysExt_RayTest(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 y2);
i32 PhysExt_QueryRay(PhysCmd_Raycast* cmd, PhysRayHit* hits, i32 maxHits);
//i32 PhysExt_QueryRay(PhysCmd_Raycast* cmd, u8* resultsBuffer, u32 bufferCapacity);
i32 PhysExt_QueryHitscan();
i32 PhysExt_QueryVolume();
Vec3 PhysExt_DebugGetPosition();

///////////////////////////////////////////////////////
// Lifecycle
///////////////////////////////////////////////////////

// Must pass two blocks of memory for the physics to use for I/O
// Size... to be determined
void PhysExt_Init(void* ptrCommandBuffer, u32 commandBufferSize, void* ptrEventBuffer, u32 eventBufferSize);
//void Phys_ReadCommands(MemoryBlock* commandBuffer);
// Returns memory block of the event buffer assigned at init
ByteBuffer PhysExt_Step(f32 deltaTime);
// Remove all objects from simulation, resetting it
void PhysExt_ClearWorld();
// Shut down library. destroy all objects.
void PhysExt_Shutdown();
