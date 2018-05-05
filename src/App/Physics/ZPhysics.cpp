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
    boxBodyCI.m_restitution = 0.3f;
    boxBodyCI.m_friction = 0.3f;

    handle->rigidBody = new btRigidBody(boxBodyCI);
    handle->rigidBody->setUserIndex(handle->id);
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
    
    btRigidBody::btRigidBodyConstructionInfo sphereBodyCI(mass, handle->motionState, handle->shape, fallInertia);
    sphereBodyCI.m_restitution = 0.75f;
    sphereBodyCI.m_friction = 0.0f;

    handle->rigidBody = new btRigidBody(sphereBodyCI);
    handle->rigidBody->setUserIndex(handle->id);
    world->dynamicsWorld->addRigidBody(handle->rigidBody);

    // DONE

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

internal void Phys_PreSolveCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep)
{
    ++g_world.debug.preSolves;
    
}

internal void Phys_PostSolveCallback(btDynamicsWorld *dynWorld, btScalar timeStep)
{
    ZBulletWorld* w = &g_world;
    ++w->debug.postSolves;

    i32 numOverlaps = 0;
    
    
    int numManifolds = dynWorld->getDispatcher()->getNumManifolds();
    g_world.debug.numManifolds = numManifolds;
    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold* contactManifold =  dynWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        Assert(numOverlaps < MAX_PHYS_OVERLAPS);
        w->overlapPairs[numOverlaps].indexA = obA->getUserIndex();
        w->overlapPairs[numOverlaps].indexB = obB->getUserIndex();
        numOverlaps++;

        int numContacts = contactManifold->getNumContacts();
        for (int j = 0; j < numContacts; j++)
        {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            if (pt.getDistance() < 0.f)
            {
                const btVector3& ptA = pt.getPositionWorldOnA();
                const btVector3& ptB = pt.getPositionWorldOnB();
                const btVector3& normalOnB = pt.m_normalWorldOnB;
            }
        }
    }
    w->numOverlaps = numOverlaps;
}

internal void Phys_WriteDebugOutput(ZBulletWorld* world)
{
    sprintf_s(g_debugString, g_debugStringSize,
"Physics debug\n\
Steps: %d\n\
PreSolves: %d\n\
PostSolves: %d\n\
Num Manifolds: %d\n\
Num Overlaps: %d\n\
",
world->debug.stepCount,
world->debug.preSolves,
world->debug.postSolves,
world->debug.numManifolds,
world->numOverlaps
);
}
