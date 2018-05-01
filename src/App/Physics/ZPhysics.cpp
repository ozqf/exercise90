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

// Delete all objects on this handle
void Phys_FreeHandle(ZBulletWorld* world, PhysBodyHandle* handle)
{
    world->dynamicsWorld->removeRigidBody(handle->rigidBody);
    handle->inUse = FALSE;
    delete handle->rigidBody;
    delete handle->motionState;
    delete handle->shape;
    *handle = {};
}

/**
 * deactivate all objects on this handle and mark it for reuse
 */
void Phys_RecycleHandle(ZBulletWorld* world, PhysBodyHandle* handle)
{
    handle->inUse = FALSE;
}

//PhysBodyHandle* Phys_CreateCuboid();

// Internal functions
PhysBodyHandle* Phys_CreateBulletSphere(ZBulletWorld* world, f32 x, f32 y, f32 z, f32 radius)
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
    handle->shape = new btSphereShape(radius);
    handle->motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(x, y, z)));
    
    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    handle->shape->calculateLocalInertia(mass, fallInertia);
    
    btRigidBody::btRigidBodyConstructionInfo sphereBodyCI(mass, handle->motionState, handle->shape, fallInertia);
    sphereBodyCI.m_restitution = 0.75f;
    sphereBodyCI.m_friction = 0.0f;

    handle->rigidBody = new btRigidBody(sphereBodyCI);
    world->dynamicsWorld->addRigidBody(handle->rigidBody);

    // TODO: Remove me: Set angular v test
    btVector3 v(0, 0, 0);
    handle->rigidBody->setLinearVelocity(v);
    // x = roll, y = pitch, z = yaw
    btVector3 angularV(0, 2, 0);
    handle->rigidBody->setAngularVelocity(angularV);
    
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

