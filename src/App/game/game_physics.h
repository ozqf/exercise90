#pragma once

#if 0
/**
 * !NO BULLET PHYSICS LIBRARY BEYOND THIS POINT!
 * 
 */
#include "../../common/com_module.h"

#include "../../../lib/bullet/btBulletCollisionCommon.h"
#include "../../../lib/bullet/btBulletDynamicsCommon.h"

#include <windows.h>

/**
 * Contains Pointers to a Bullet Physics Shape
 */
struct PhysBodyHandle
{
    u8 inUse;
    i32 id;
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

ZBulletWorld g_world;
PhysicsTestState g_physTest;

Vec3 g_testPos;

#define MAX_PHYS_BODIES 2048

global_variable PhysBodyHandle g_bodies[MAX_PHYS_BODIES];

PhysBodyHandle* PHYS_GetFreeBodyHandle(PhysBodyList* list)
{
    
    for(i32 i = 0; i < list->capacity; ++i)
    {
        PhysBodyHandle* handle = &list->items[i];
        if (!handle->inUse)
        {
            handle->inUse = 1;
            return handle;
        }
    }
    return NULL;
}

PhysBodyHandle* PHYS_CreateRigidBody(ZBulletWorld* world)
{
    PhysBodyHandle* handle = PHYS_GetFreeBodyHandle(&world->bodies);
    Assert(handle != NULL);



    return handle;
}

void PHYS_Init()
{
    g_physTest = {};
    g_world = {};
    g_world.bodies.items = g_bodies;
    g_world.bodies.capacity = MAX_PHYS_BODIES;

    g_world.broadphase = new btDbvtBroadphase();

    g_world.collisionConfiguration = new btDefaultCollisionConfiguration();
    g_world.dispatcher = new btCollisionDispatcher(g_world.collisionConfiguration);

    g_world.solver = new btSequentialImpulseConstraintSolver();

    g_world.dynamicsWorld = new btDiscreteDynamicsWorld(g_world.dispatcher, g_world.broadphase, g_world.solver, g_world.collisionConfiguration);

    g_world.dynamicsWorld->setGravity(btVector3(0, -15, 0));


    // hello bullet physics

    // Add static ground-plane
    g_physTest.groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), -1);
    g_physTest.groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, g_physTest.groundMotionState, g_physTest.groundShape, btVector3(0, 0, 0));
    groundRigidBodyCI.m_restitution = 1.0f;
    g_physTest.groundRigidBody = new btRigidBody(groundRigidBodyCI);
    g_world.dynamicsWorld->addRigidBody(g_physTest.groundRigidBody);

    // Add dynamic sphere, 50m above the ground
    g_physTest.sphereShape = new btSphereShape(1);
    g_physTest.sphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 15, 0)));
    
    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    g_physTest.sphereShape->calculateLocalInertia(mass, fallInertia);

    btRigidBody::btRigidBodyConstructionInfo sphereBodyCI(mass, g_physTest.sphereMotionState, g_physTest.sphereShape, fallInertia);
    sphereBodyCI.m_restitution = 0.75f;
    sphereBodyCI.m_friction = 0.0f;
    
    g_physTest.sphereRigidBody = new btRigidBody(sphereBodyCI);
    //g_physTest.sphereRigidBody->setRestitution(1);
    g_world.dynamicsWorld->addRigidBody(g_physTest.sphereRigidBody);
}

void PHYS_Step(GameState* gs, GameTime* time)
{
    g_world.dynamicsWorld->stepSimulation(time->deltaTime, 10);
    btTransform t;
    g_physTest.sphereRigidBody->getMotionState()->getWorldTransform(t);

    btVector3 pos = t.getOrigin();
    g_testPos.x = pos.getX();
    g_testPos.y = pos.getY();
    g_testPos.z = pos.getZ();

    char buf[128];
        sprintf_s(buf, 128, "Sphere pos: %.2f, %.2f, %.2f\n", pos.getX(), pos.getY(), pos.getZ());
        OutputDebugString(buf);
    
}

/*

Example of deleting full physics world:

https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=1660

"When a rigid body is added to the world, you should not delete it, until you after you removed it.
In general, delete objects in reverse order of creation, for example see BasicDemo: "

void	BasicDemo::exitPhysics()
{
	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}

	//delete dynamics world
	delete m_dynamicsWorld;

	//delete solver
	delete m_solver;

	//delete broadphase
	delete m_overlappingPairCache;

	//delete dispatcher
	delete m_dispatcher;

	delete m_collisionConfiguration;

	
}
*/

void PHYS_Shutdown()
{
    // Get order right or it will cause an access violation
	delete g_world.dynamicsWorld;
	delete g_world.solver;
	delete g_world.dispatcher;
	delete g_world.collisionConfiguration;
    delete g_world.broadphase;
}

#endif
