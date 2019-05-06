#ifndef PHYSICS_INTERFACE_H
#define PHYSICS_INTERFACE_H

/**
 * Collision API
 * Interface to physics engine
 * > PhysCmd == enqueue commands that are executed when the next step is called
 * > PhysExt == unqueued, external and executed immediately
 * 
 * Phys_Bullet
 * 
 */
#include "../common/common.h"

#include "physics_external_types.h"

//////////////////////////////////////////////////////////////////
// Queued commands
//////////////////////////////////////////////////////////////////
extern "C"
i32 PhysCmd_CreateShape(WorldHandle* handle, ZShapeDef* def, u32 ownerId);
extern "C"
i32 PhysCmd_CreateBox(
    WorldHandle* handle,
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
extern "C" void PhysCmd_RemoveShape(WorldHandle* handle, i32 shapeId);
extern "C" void PhysCmd_SetState(WorldHandle* handle, PhysCmd_State* state);
extern "C" void PhysCmd_TeleportShape(WorldHandle* handle, i32 shapeId, f32 posX, f32 posY, f32 posZ);
extern "C" void PhysCmd_ChangeVelocity(WorldHandle* handle, i32 shapeId, f32 velX, f32 velY, f32 velZ);

//////////////////////////////////////////////////////////////////
// Instant commands
//////////////////////////////////////////////////////////////////
extern "C" void PhysExt_GetDebugString(char** str, i32* length);
extern "C" i32 PhysExt_RayTest(WorldHandle* handle, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 y2);
extern "C" i32 PhysExt_QueryRay(WorldHandle* handle, PhysCmd_Raycast* cmd, PhysRayHit* hits, i32 maxHits);
//extern "C" i32 PhysExt_QueryRay(PhysCmd_Raycast* cmd, u8* resultsBuffer, u32 bufferCapacity);
extern "C" i32 PhysExt_QueryHitscan();
extern "C" i32 PhysExt_QueryVolume();
extern "C" Vec3 PhysExt_DebugGetPosition();

///////////////////////////////////////////////////////
// Lifecycle
///////////////////////////////////////////////////////

extern "C" WorldHandle* PhysExt_Create(PhysErrorHandler errorHandler);
//void Phys_ReadCommands(MemoryBlock* commandBuffer);
// Returns memory block of the event buffer assigned at init
extern "C" ByteBuffer PhysExt_Step(WorldHandle* handle, f32 deltaTime);
// Remove all objects from simulation, resetting it
extern "C" void PhysExt_ClearWorld(WorldHandle* handle);
// Shut down library. destroy all objects.
extern "C" void PhysExt_Shutdown(WorldHandle* handle);

#endif //PHYSICS_INTERFACE_H