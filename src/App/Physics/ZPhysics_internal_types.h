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
    // internal id, matches user index on btRigidbody
    i32 id;
    i32 group;
    u16 mask;
    // external id
    u16 ownerId;
    u16 ownerIteration;
    
    ZShapeDef def;
    
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

struct PhysDebugInfo
{
    u32 stepCount = 0;
    u32 preSolves = 0;
    u32 postSolves = 0;

    u32 numManifolds = 0;
};

struct PhysOverlapPair
{
    i32 indexA;
    i32 indexB;
};

struct PhysInfinitePlane
{
    
};

struct ZBulletWorld
{
    PhysBodyList bodies;
    PhysDebugInfo debug;

    PhysOverlapPair* overlapPairs;
    i32 numOverlaps;
    i32 nextQueryId = 0;

    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;

    btDiscreteDynamicsWorld* dynamicsWorld;
};
#if 0
struct PhysicsTestState
{
    btCollisionShape* groundShape;
    btDefaultMotionState* groundMotionState;
    btRigidBody* groundRigidBody;

    btCollisionShape* sphereShape;
    btDefaultMotionState* sphereMotionState;
    btRigidBody* sphereRigidBody;
};
#endif
