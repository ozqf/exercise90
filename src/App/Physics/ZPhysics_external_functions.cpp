#pragma once

#include "../../Shared/shared_module.h"

/////////////////////////////////////////////////////////////
// List operations
/////////////////////////////////////////////////////////////
int Phys_CreateSphere(f32 x, f32 y, f32 z, f32 radius, u16 ownerId, u16 ownerIteration)
{
    PhysBodyHandle* h = Phys_CreateBulletSphere(&g_world, x, y, z, radius);
	h->ownerId = ownerId;
	h->ownerIteration = ownerIteration;
    return h->id;
}

int Phys_RemoveShape()
{
    return 0;
}

/////////////////////////////////////////////////////////////
// Querying
/////////////////////////////////////////////////////////////
int Phys_QueryHitscan()
{
    return 0;
}

int Phys_QueryVolume()
{
    return 0;
}

Vec3 Phys_DebugGetPosition()
{
    return g_testPos;
}

/////////////////////////////////////////////////////////////
// Lifetime
/////////////////////////////////////////////////////////////

void Phys_CreateTestScene(ZBulletWorld* world)
{
    // hello bullet physics

    // Add static ground-plane
    g_physTest.groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), -1);
    g_physTest.groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, g_physTest.groundMotionState, g_physTest.groundShape, btVector3(0, 0, 0));
    groundRigidBodyCI.m_restitution = 1.0f;
    g_physTest.groundRigidBody = new btRigidBody(groundRigidBodyCI);
    world->dynamicsWorld->addRigidBody(g_physTest.groundRigidBody);

#if 0
    // Add dynamic sphere, 50m above the ground
    g_physTest.sphereShape = new btSphereShape(1);
    g_physTest.sphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0.2f, 12, 0.5f)));
    
    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    g_physTest.sphereShape->calculateLocalInertia(mass, fallInertia);

    btRigidBody::btRigidBodyConstructionInfo sphereBodyCI(mass, g_physTest.sphereMotionState, g_physTest.sphereShape, fallInertia);
    sphereBodyCI.m_restitution = 0.75f;
    sphereBodyCI.m_friction = 0.0f;
    
    g_physTest.sphereRigidBody = new btRigidBody(sphereBodyCI);
    //g_physTest.sphereRigidBody->setRestitution(1);
    world->dynamicsWorld->addRigidBody(g_physTest.sphereRigidBody);
#endif
}

void Phys_Init()
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
    
    Phys_CreateTestScene(&g_world);
}

void Phys_StepWorld(ZBulletWorld* world, MemoryBlock* eventBuffer, f32 deltaTime)
{
    world->dynamicsWorld->stepSimulation(deltaTime, 10);
#if 0
    btTransform trans;
    g_physTest.sphereRigidBody->getMotionState()->getWorldTransform(trans);

    btVector3 pos = trans.getOrigin();
    g_testPos.x = pos.getX();
    g_testPos.y = pos.getY();
    g_testPos.z = pos.getZ();
#endif
    u8* writePosition = (u8*)eventBuffer->ptrMemory;
    i32 len = world->bodies.capacity;
    for (int i = 0; i < len; ++i)
    {
        PhysBodyHandle* h = &world->bodies.items[i];
        if (h->inUse == FALSE) { continue; }
        Assert(h->motionState != NULL);

        // Get position
        btTransform t;
        //h->motionState->getWorldTransform(t);
        h->rigidBody->getMotionState()->getWorldTransform(t);
        btVector3 p = t.getOrigin();

        // Write position to handle
        h->transform.pos.x = p.getX();
        h->transform.pos.y = p.getY();
        h->transform.pos.z = p.getZ();

        // write transform update
        ZTransformUpdateEvent tUpdate = {};
        tUpdate.type = 1;
        tUpdate.ownerId = h->ownerId;
		tUpdate.ownerIteration = h->ownerIteration;
        tUpdate.pos.x = h->transform.pos.x;
        tUpdate.pos.y = h->transform.pos.y;
        tUpdate.pos.z = h->transform.pos.z;
        
        // attempt to read angle:

        // Bullet Appears to use X = roll, Y = pitch, Z = Yaw
        // Ex90 uses x = pitch, y = yaw, z = roll
        //          Bullet      Ex90
        // Roll     X           Z
        // Pitch    Y           X
        // Yaw      Z           Y

#if 1
        btScalar openglM[16];
        t.getOpenGLMatrix(openglM);

        // btScalar m[16];
        // t.getOpenGLMatrix(m);
        f32 fAngZ = atan2f(openglM[1], openglM[5]);
        f32 fAngY = atan2f(openglM[8], openglM[10]);
        f32 fAngX = -asinf(openglM[9]);
        // tUpdate.rot.x = COM_CapAngleDegrees(fAngX * RAD2DEG);
        // tUpdate.rot.y = COM_CapAngleDegrees(fAngY * RAD2DEG);
        // tUpdate.rot.z = COM_CapAngleDegrees(fAngZ * RAD2DEG);
        tUpdate.rot.x = fAngX * RAD2DEG;
        tUpdate.rot.y = fAngY * RAD2DEG;
        tUpdate.rot.z = fAngZ * RAD2DEG;
#endif
#if 0
        btMatrix3x3 m = t.getBasis();
        btScalar pitchX;
        btScalar yawY;
        btScalar rollZ;
        m.getEulerYPR(yawY, pitchX, rollZ);
        m.getEulerZYX(yawY, pitchX, rollZ);
        tUpdate.rot.x = COM_CapAngleDegrees(pitchX * RAD2DEG);
        tUpdate.rot.y = COM_CapAngleDegrees(yawY * RAD2DEG);
        tUpdate.rot.z = COM_CapAngleDegrees(rollZ * RAD2DEG);

        // tUpdate.rot.x = pitchX * RAD2DEG;
        // tUpdate.rot.y = yawY * RAD2DEG;
        // tUpdate.rot.z = rollZ * RAD2DEG;
    
#endif
        COM_COPY_STEP(&tUpdate, writePosition, ZTransformUpdateEvent);
    }
    
    // Mark end of buffer
    *writePosition = 0;

    /*char buf[128];
        sprintf_s(buf, 128, "Sphere pos: %.2f, %.2f, %.2f\n", pos.getX(), pos.getY(), pos.getZ());
        OutputDebugString(buf);*/
    
}

void Phys_Step(MemoryBlock* eventBuffer, f32 deltaTime)
{
    Phys_StepWorld(&g_world, eventBuffer, deltaTime);
}

void Phys_Shutdown()
{
    // Get order right or it will cause an access violation
	delete g_world.dynamicsWorld;
	delete g_world.solver;
	delete g_world.dispatcher;
	delete g_world.collisionConfiguration;
    delete g_world.broadphase;
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
