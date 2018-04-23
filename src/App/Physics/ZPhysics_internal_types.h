#pragma once

#include "ZPhysics_interface.h"

#include "../../../lib/bullet/btBulletCollisionCommon.h"
#include "../../../lib/bullet/btBulletDynamicsCommon.h"


/**
 * Contains Pointers to a Bullet Physics Shape
 */
struct PhysBodyHandle
{
    u8 inUse;
    // internal id
    i32 id;
    // external id
    u16 ownerId;
    u16 ownerIteration;
    // written every frame, report state to outside world
    Transform transform;
    //ZCollider collider; // 'shadow' presented to the external interface

    // pointers to bullet physics objects
    btCollisionShape* shape;
    btDefaultMotionState* motionState;
    btRigidBody* rigidBody;
};

struct PhysBodyList
{
    PhysBodyHandle* items;
    i32 capacity;
};

struct ZBulletWorld
{
    PhysBodyList bodies;

    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;

    btDiscreteDynamicsWorld* dynamicsWorld;
};

struct PhysicsTestState
{
    btCollisionShape* groundShape;
    btDefaultMotionState* groundMotionState;
    btRigidBody* groundRigidBody;

    btCollisionShape* sphereShape;
    btDefaultMotionState* sphereMotionState;
    btRigidBody* sphereRigidBody;
};
