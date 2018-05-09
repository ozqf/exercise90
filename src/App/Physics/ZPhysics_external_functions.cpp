#pragma once

#include "../../common/com_defines.h"

/////////////////////////////////////////////////////////////
// ISSUE COMMAND
/////////////////////////////////////////////////////////////

i32 Phys_CreateShape(ZShapeDef* def)
{
    Assert(def != NULL);
    PhysBodyHandle* h = Phys_GetFreeBodyHandle(&g_world.bodies);
    
    Assert(h != NULL);
    def->handleId = h->id;
    g_cmdBuf.ptrWrite += COM_WriteByte(Create, g_cmdBuf.ptrWrite);
    g_cmdBuf.ptrWrite += COM_COPY(def, g_cmdBuf.ptrWrite, ZShapeDef);
    return h->id;
}

i32 Phys_CreateBox(
    f32 x,
    f32 y,
    f32 z,
    f32 halfSizeX,
    f32 halfSizeY,
    f32 halfSizeZ,
    u32 flags,
    u16 group,
    u16 mask,
    u16 ownerId,
    u16 ownerIteration
    )
{
    ZShapeDef def = {};
    def.shapeType = box;
    def.flags = flags;
    def.group = group;
    def.mask = mask;
    def.pos[0] = x;
    def.pos[1] = y;
    def.pos[2] = z;
    def.data.box.halfSize[0] = halfSizeX;
    def.data.box.halfSize[1] = halfSizeY;
    def.data.box.halfSize[2] = halfSizeZ;
    return Phys_CreateShape(&def);
}

i32 Phys_RemoveShape()
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

i32 Phys_QueryHitscan()
{
    return 0;
}

i32 Phys_QueryVolume()
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

// If the provided command buffer is NULL or the size is zero, fail
void Phys_Init(void* ptrCommandBuffer, u32 commandBufferSize)
{
    if (commandBufferSize == 0 || ptrCommandBuffer == NULL)
    {
        Assert(false);
    }
    g_cmdBuf = {};
    g_cmdBuf.ptrStart = (u8*)ptrCommandBuffer;
    g_cmdBuf.ptrEnd = g_cmdBuf.ptrStart;
    g_cmdBuf.ptrWrite = g_cmdBuf.ptrStart;
    g_cmdBuf.size = commandBufferSize;

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

void Phys_Step(MemoryBlock* eventBuffer, f32 deltaTime)
{
    Phys_StepWorld(&g_world, eventBuffer, deltaTime);
    Phys_WriteDebugOutput(&g_world);
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
