#pragma once

// header, contains data structures and function definitions
#include "ZPhysics_interface.h"
/**
 * !NO BULLET PHYSICS LIBRARY ABOVE THIS POINT!
 * 
 */
#include "../../../lib/bullet/btBulletCollisionCommon.h"
#include "../../../lib/bullet/btBulletDynamicsCommon.h"

#include "../../../lib/bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

// For debug output ONLY
#include <windows.h>

// Internal data structures used to interact with Bullet Physics
//#include "ZPhysics_internal_types.h"
#include "ZPhysics_internal_interface.h"

// Global variables used by the rest of the internal system
#include "ZPhysics_globals.h"

// Implement public interface
#include "ZPhysics_external_functions.cpp"

#define PHYS_DEFAULT_FRICTION 1.0
#define PHYS_DEFAULT_RESTITUTION 0.0

////////////////////////////////////////////////////////////////////////////////////////////
// MANAGE HANDLES
////////////////////////////////////////////////////////////////////////////////////////////
PhysBodyHandle* PHYS_GetFreeBodyHandle(PhysBodyList* list)
{
    
    for(i32 i = 0; i < list->capacity; ++i)
    {
        PhysBodyHandle* handle = &list->items[i];
        if (handle->inUse == FALSE)
        {
            handle->id = i;
            handle->inUse = TRUE;
            return handle;
        }
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CREATE SHAPES
////////////////////////////////////////////////////////////////////////////////////////////
// Delete all objects on this handle
void Phys_FreeHandle(ZBulletWorld* world, PhysBodyHandle* handle)
{
    // TODO: This is exactly how to free stuff in bullet, find proper example
    // Assuming that on shutdown we will be removing freeing
    // shape handles that were never initialised to begin with
    if (handle->rigidBody != NULL)
    {
        world->dynamicsWorld->removeRigidBody(handle->rigidBody);
        delete handle->rigidBody;
    }
    if (handle->motionState != NULL)
    {
        delete handle->motionState;
    }
    if (handle->shape != NULL)
    {
        delete handle->shape;
    }
    
    handle->inUse = FALSE;
    *handle = {};
}

/**
 * deactivate all objects on this handle and mark it for reuse
 */
void Phys_RecycleHandle(ZBulletWorld* world, PhysBodyHandle* handle)
{
    handle->inUse = FALSE;
}

PhysBodyHandle* Phys_CreateBulletBox(ZBulletWorld* world, ZBoxDef def)
{
    PhysBodyHandle* handle = PHYS_GetFreeBodyHandle(&world->bodies);
    Assert(handle != NULL);

    handle->shape = new btBoxShape(btVector3(def.halfSize[0], def.halfSize[1], def.halfSize[2]));
    handle->motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(def.base.pos[0], def.base.pos[1], def.base.pos[2])));

    btScalar mass;
    if (def.base.flags & ZCOLLIDER_FLAG_STATIC)
    {
        mass = 0;
    }
    else
    {
        mass = 1;
    }

    btVector3 fallInertia(0, 0, 0);
    handle->shape->calculateLocalInertia(mass, fallInertia);

    btRigidBody::btRigidBodyConstructionInfo boxBodyCI(mass, handle->motionState, handle->shape, fallInertia);
    //boxBodyCI.m_restitution = PHYS_DEFAULT_RESTITUTION;
    //boxBodyCI.m_friction = PHYS_DEFAULT_FRICTION;
    
    handle->rigidBody = new btRigidBody(boxBodyCI);

    i32 btFlags = 0;
    if (def.base.flags & ZCOLLIDER_FLAG_KINEMATIC)
    {
        btFlags |= CF_KINEMATIC_OBJECT;
        // Restrict motion to specific axes (in this case, only move on X/Z)
        //handle->rigidBody->setLinearFactor(btVector3(1, 0, 1));
    }
    if (def.base.flags & ZCOLLIDER_FLAG_NO_ROTATION)
    {
        handle->rigidBody->setAngularFactor(btVector3(0, 0, 0));
    }
    
    handle->rigidBody->setUserIndex(handle->id);
    handle->rigidBody->setCollisionFlags(btFlags);

    world->dynamicsWorld->addRigidBody(handle->rigidBody);

    return handle;
}

PhysBodyHandle* Phys_CreateBulletSphere(ZBulletWorld* world, ZSphereDef def)
{
    PhysBodyHandle* handle = PHYS_GetFreeBodyHandle(&world->bodies);
    Assert(handle != NULL);
    /*
    u8 inUse;
    i32 id;
    btCollisionShape* shape;
    btDefaultMotionState* motionState;
    btRigidBody* rigidBody;
    */
    handle->shape = new btSphereShape(def.radius);
    handle->motionState = new btDefaultMotionState(
        btTransform(
            btQuaternion(0, 0, 0, 1),
            btVector3(def.base.pos[0], def.base.pos[1], def.base.pos[2])
            )
        );
    
    btScalar mass;
    if (def.base.flags & ZCOLLIDER_FLAG_STATIC)
    {
        mass = 0;
    }
    else
    {
        mass = 1;
    }
    btVector3 fallInertia(0, 0, 0);
    handle->shape->calculateLocalInertia(mass, fallInertia);
    
    btRigidBody::btRigidBodyConstructionInfo sphereBodyCI(mass, handle->motionState, handle->shape, fallInertia);
    //sphereBodyCI.m_restitution = PHYS_DEFAULT_RESTITUTION;
    //sphereBodyCI.m_friction = PHYS_DEFAULT_FRICTION;

    i32 btFlags = 0;
    if (def.base.flags & ZCOLLIDER_FLAG_KINEMATIC)
    {
        btFlags |= CF_KINEMATIC_OBJECT;
    }
    if (def.base.flags & ZCOLLIDER_FLAG_NO_ROTATION)
    {
        handle->rigidBody->setAngularFactor(btVector3(0, 0, 0));
    }

    handle->rigidBody = new btRigidBody(sphereBodyCI);
    handle->rigidBody->setUserIndex(handle->id);
    handle->rigidBody->setCollisionFlags(btFlags);
    world->dynamicsWorld->addRigidBody(handle->rigidBody);

    // DONE
#if 0
    // TODO: Remove me: Set angular v test
    btVector3 v(0, 0, 0);
    handle->rigidBody->setLinearVelocity(v);
    // x = roll, y = pitch, z = yaw
    btVector3 angularV(0, 2, 0);
    handle->rigidBody->setAngularVelocity(angularV);
#endif
    // Create sensor
    // btGhostObject* ghost = new btGhostObject();
    // ghost->setCollisionShape(new btSphereShape(radius));
    // ghost->setWorldTransform(btTransform());

    // Init Ghost object for collision detection
    
    // handle->inUse;
    // handle->id;
    // handle->shape;
    // handle->motionState;
    // handle->rigidBody;

    return handle;
}

PhysBodyHandle* Phys_CreateBulletInfinitePlane(ZBulletWorld* world, ZShapeDef def)
{
    PhysBodyHandle* handle = PHYS_GetFreeBodyHandle(&world->bodies);
    Assert(handle != NULL);

    handle->shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    handle->motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, def.pos[1], 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, handle->motionState, handle->shape, btVector3(0, 0, 0));
    //groundRigidBodyCI.m_restitution = PHYS_DEFAULT_RESTITUTION;
    //groundRigidBodyCI.m_friction = PHYS_DEFAULT_FRICTION;
    handle->rigidBody = new btRigidBody(groundRigidBodyCI);
    world->dynamicsWorld->addRigidBody(handle->rigidBody);
    handle->rigidBody->setUserIndex(handle->id);
    return handle;
}

internal i32 Phys_FindOverlapPair(ZBulletWorld* world, i32 a, i32 b)
{
    for (int i = 0; i < world->numOverlaps; ++i)
    {
        // order of indices doesn't matter
        if (
            (world->overlapPairs[i].indexA == a && world->overlapPairs[i].indexB == b)
            ||
            (world->overlapPairs[i].indexA == b && world->overlapPairs[i].indexB == a)
            )
        {
            return i;
        }
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CALLBACKS
////////////////////////////////////////////////////////////////////////////////////////////
internal void Phys_PreSolveCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep)
{
    ++g_world.debug.preSolves;
    
}

internal void Phys_PostSolveCallback(btDynamicsWorld *dynWorld, btScalar timeStep)
{
    ZBulletWorld* w = &g_world;
    ++w->debug.postSolves;

    // Reset overlap list and rebuild
    w->numOverlaps = 0;

    int numManifolds = dynWorld->getDispatcher()->getNumManifolds();
    g_world.debug.numManifolds = numManifolds;
    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold* contactManifold =  dynWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        i32 indexA = obA->getUserIndex();
        i32 indexB = obB->getUserIndex();
        
        int numContacts = contactManifold->getNumContacts();
        u8 areTouching = 0;
        
        for (int j = 0; j < numContacts; j++)
        {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            //if (pt.getDistance() < 0.05f)
            if (pt.getDistance() < F32_EPSILON)
            {
                areTouching = 1;
                const btVector3& ptA = pt.getPositionWorldOnA();
                const btVector3& ptB = pt.getPositionWorldOnB();
                const btVector3& normalOnB = pt.m_normalWorldOnB;
            }
        }

        if (areTouching)
        {
            if (Phys_FindOverlapPair(w, indexA, indexB) >= 0) { continue; }

            Assert(w->numOverlaps < MAX_PHYS_OVERLAPS);
            w->overlapPairs[w->numOverlaps].indexA = indexA;
            w->overlapPairs[w->numOverlaps].indexB = indexB;
            w->numOverlaps++;
        
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG
////////////////////////////////////////////////////////////////////////////////////////////
internal void Phys_WriteDebugOutput(ZBulletWorld* world)
{
    char* ptr = g_debugString;
    i32 remaining = g_debugStringSize;
    i32 written = 0;
    
    written = sprintf_s(ptr, remaining,
"Physics debug\n\
Steps: %d\n\
PreSolves: %d\n\
PostSolves: %d\n\
Num Manifolds: %d\n\
Num Overlaps: %d\n\
--Collision Pairs--\n\
",
world->debug.stepCount,
world->debug.preSolves,
world->debug.postSolves,
world->debug.numManifolds,
world->numOverlaps
);
 //   ptr += written;
 //   remaining -= written;

 //   for (int i = 0; i < world->numOverlaps; ++i)
 //   {
 //       written = sprintf_s(ptr, remaining, "(%d vs %d)\n", world->overlapPairs[i].indexA, world->overlapPairs[i].indexB);
 //       //written = sprintf_s(ptr, remaining, "(%d vs %d)\n", 1, 2);
 //       //written = sprintf_s(ptr, remaining, "12345");
 //       ptr += written;
 //       remaining -= written;
 //       if (remaining <= 0) { break; }
 //   }
	//ptr = NULL;
}
