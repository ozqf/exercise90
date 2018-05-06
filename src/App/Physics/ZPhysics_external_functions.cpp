#pragma once

#include "../../common/com_defines.h"

/////////////////////////////////////////////////////////////
// List operations
/////////////////////////////////////////////////////////////


int Phys_CreateSphere(f32 x, f32 y, f32 z, f32 radius, u32 flags, u16 ownerId, u16 ownerIteration)
{
    ZSphereDef def = {};
    def.base.type = ZCOLLIDER_TYPE_SPHERE;
    def.base.pos[0] = x;
    def.base.pos[1] = y;
    def.base.pos[2] = z;
    def.base.flags = flags;
    def.radius = radius;

    PhysBodyHandle* h = Phys_CreateBulletSphere(&g_world, def);
	h->ownerId = ownerId;
	h->ownerIteration = ownerIteration;
    return h->id;
}

int Phys_CreateBox(f32 x, f32 y, f32 z, f32 halfSizeX, f32 halfSizeY, f32 halfSizeZ, u32 flags, u16 ownerId, u16 ownerIteration)
{
    ZBoxDef def = {};
    def.base.type = ZCOLLIDER_TYPE_CUBOID;
    def.base.pos[0] = x;
    def.base.pos[1] = y;
    def.base.pos[2] = z;
    def.base.flags = flags;
    def.halfSize[0] = halfSizeX;
    def.halfSize[1] = halfSizeY;
    def.halfSize[2] = halfSizeZ;
    
    PhysBodyHandle* h = Phys_CreateBulletBox(&g_world, def);
	h->ownerId = ownerId;
	h->ownerIteration = ownerIteration;
    return h->id;
}

i32 Phys_CreateInfinitePlane(f32 y, u16 ownerId, u16 ownerIteration)
{
    ZShapeDef def = {};
    def.type = ZCOLLIDER_TYPE_INFINITE_PLANE;
    def.pos[0] = 0;
    def.pos[1] = y;
    def.pos[2] = 0;
    PhysBodyHandle* h = Phys_CreateBulletInfinitePlane(&g_world, def);
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
void Phys_GetDebugString(char** str, i32* length)
{
    *str = g_debugString;
    *length = g_debugStringSize;
}

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

void Phys_CreateTestScene(ZBulletWorld* world)
{
#if 0
    // hello bullet physics
    // Add static ground-plane
    g_physTest.groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), -1);
    g_physTest.groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, g_physTest.groundMotionState, g_physTest.groundShape, btVector3(0, 0, 0));
    groundRigidBodyCI.m_restitution = 1.0f;
    g_physTest.groundRigidBody = new btRigidBody(groundRigidBodyCI);
    world->dynamicsWorld->addRigidBody(g_physTest.groundRigidBody);


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

/////////////////////////////////////////////////////////////
// Lifetime
/////////////////////////////////////////////////////////////

void Phys_Init()
{
    //g_physTest = {};
    g_world = {};
    g_world.bodies.items = g_bodies;
    g_world.bodies.capacity = MAX_PHYS_BODIES;
    g_world.overlapPairs = g_overlapPairs;
    g_world.numOverlaps = MAX_PHYS_OVERLAPS;

    g_world.broadphase = new btDbvtBroadphase();

    g_world.collisionConfiguration = new btDefaultCollisionConfiguration();
    g_world.dispatcher = new btCollisionDispatcher(g_world.collisionConfiguration);

    g_world.solver = new btSequentialImpulseConstraintSolver();

    g_world.dynamicsWorld = new btDiscreteDynamicsWorld(g_world.dispatcher, g_world.broadphase, g_world.solver, g_world.collisionConfiguration);

    g_world.dynamicsWorld->setGravity(btVector3(0, -15, 0));

	g_world.dynamicsWorld->setInternalTickCallback(Phys_PreSolveCallback, 0, true);
    g_world.dynamicsWorld->setInternalTickCallback(Phys_PostSolveCallback, 0, false);
    
    Phys_CreateTestScene(&g_world);
}

void Phys_ReadCommands(MemoryBlock* commandBuffer)
{
    //Phys_TickCallback(g_world.dynamicsWorld, 0.016f);
}

internal void Phys_StepWorld(ZBulletWorld* world, MemoryBlock* eventBuffer, f32 deltaTime)
{
    ++world->debug.stepCount;
    world->dynamicsWorld->stepSimulation(deltaTime, 10);

    u8* writePosition = (u8*)eventBuffer->ptrMemory;
    i32 len = world->bodies.capacity;
    for (int i = 0; i < len; ++i)
    {
        PhysBodyHandle* h = &world->bodies.items[i];
        if (h->inUse == FALSE) { continue; }
        Assert(h->motionState != NULL);

        // Get position
        btTransform t;
        h->rigidBody->getMotionState()->getWorldTransform(t);
        
        // write transform update
        ZTransformUpdateEvent tUpdate = {};
        tUpdate.type = 1;
        tUpdate.ownerId = h->ownerId;
		tUpdate.ownerIteration = h->ownerIteration;
        
        btScalar openglM[16];
        t.getOpenGLMatrix(openglM);
        for (i32 j = 0; j < 16; ++j)
        {
            tUpdate.matrix[j] = openglM[j];
        }
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
    Phys_WriteDebugOutput(&g_world);
}

void Phys_Shutdown()
{
    for (int i = 0; i < g_world.bodies.capacity; ++i)
    {
        Phys_FreeHandle(&g_world, &g_world.bodies.items[i]);
    }
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
