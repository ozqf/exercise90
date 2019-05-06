#pragma once

#include "ZPhysics_module.cpp"

void Phys_Error(char* message)
{
    printf("PHYS Error: %s\n", message);
    if (g_errorHandler)
    {
        g_errorHandler(message);
    }
    ILLEGAL_CODE_PATH
}

/////////////////////////////////////////////////////////////
// ISSUE COMMAND
/////////////////////////////////////////////////////////////

#if 0
i32 Phys_EnqueueRaycast(PhysCmd_Raycast* ev)
{
    // Thread safe...? do you care...?
    i32 id = g_world.nextQueryId++;
    ev->id = id;
    g_input.ptrWrite += COM_WriteByte(Raycast, g_input.ptrWrite);
    g_input.ptrWrite += COM_COPY_STRUCT(ev, g_input.ptrWrite, PhysCmd_Raycast);
    return id;
}
#endif

i32 PhysCmd_CreateShape(ZShapeDef* def, u32 externalId)
{
    COM_ASSERT(def != NULL, "No shape def provided");
    PhysBodyHandle* h = Phys_GetFreeBodyHandle(&g_world.bodies);
    
    COM_ASSERT(h != NULL, "No free body handle");
    def->handleId = h->id;
	h->externalId = externalId;
    g_input.ptrWrite += COM_WriteByte(Create, g_input.ptrWrite);
    g_input.ptrWrite += COM_COPY_STRUCT(def, g_input.ptrWrite, ZShapeDef);
    if (g_world.verbose)
    {
        printf("PHYS Assigning shape Id %d for external %d\n", def->handleId, h->externalId);
    }
    
    return h->id;
}
#if 0
i32 PhysCmd_CreateBox(
    f32 x,
    f32 y,
    f32 z,
    f32 halfSizeX,
    f32 halfSizeY,
    f32 halfSizeZ,
    u32 flags,
    i32 tag,
    u16 group,
    u16 mask,
    u32 ownerId
    )
{
    ZShapeDef def = {};
    def.shapeType = box;
    def.flags = flags;
    def.group = group;
    def.mask = mask;
    def.tag = tag;
    def.pos[0] = x;
    def.pos[1] = y;
    def.pos[2] = z;
    def.data.box.halfSize[0] = halfSizeX;
    def.data.box.halfSize[1] = halfSizeY;
    def.data.box.halfSize[2] = halfSizeZ;
    return PhysCmd_CreateShape(&def, ownerId);
}
#endif
void PhysCmd_RemoveShape(i32 shapeId)
{
    PhysBodyHandle* h = Phys_GetHandleById(&g_world.bodies, shapeId);
    g_input.ptrWrite += COM_WriteByte(Remove, g_input.ptrWrite);
    g_input.ptrWrite += COM_WriteI32(shapeId, g_input.ptrWrite);
}

void PhysCmd_SetState(PhysCmd_State* state)
{
    g_input.ptrWrite += COM_WriteByte(SetState, g_input.ptrWrite);
    g_input.ptrWrite += COM_COPY_STRUCT(state, g_input.ptrWrite, PhysCmd_State);
}

void PhysCmd_TeleportShape(i32 shapeId, f32 posX, f32 posY, f32 posZ)
{
    PhysCmd_Teleport cmd = {};
    cmd.shapeId = shapeId;
    cmd.pos[0] = posX;
    cmd.pos[1] = posY;
    cmd.pos[2] = posZ;
	
    g_input.ptrWrite += COM_WriteByte(Teleport, g_input.ptrWrite);
    g_input.ptrWrite += COM_COPY_STRUCT(&cmd, g_input.ptrWrite, PhysCmd_Teleport);
}

void PhysCmd_ChangeVelocity(i32 shapeId, f32 velX, f32 velY, f32 velZ)
{
	PhysCmd_VelocityChange cmd = {};
	cmd.shapeId = shapeId;
	cmd.vel[0] = velX;
	cmd.vel[1] = velY;
	cmd.vel[2] = velZ;

	g_input.ptrWrite += COM_WriteByte(SetVelocity, g_input.ptrWrite);
	g_input.ptrWrite += COM_COPY_STRUCT(&cmd, g_input.ptrWrite, PhysCmd_VelocityChange);
}

/////////////////////////////////////////////////////////////
// Querying
/////////////////////////////////////////////////////////////

// return number of hits or number of hits written if max is lower
i32 PhysExt_QueryRay(PhysCmd_Raycast* cmd, PhysRayHit* hits, i32 maxHits)
{
    return Phys_QuickRaycast(&g_world, cmd, hits, maxHits);
}

// Return bytes written
// i32 PhysExt_QueryRay(PhysCmd_Raycast* cmd, u8* resultsBuffer, u32 bufferCapacity)
// {
//     return Phys_ExecRaycast(&g_world, cmd, resultsBuffer, bufferCapacity);
// }

i32 PhysExt_RayTest(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 y2)
{
    return PhysCmd_RayTest(&g_world, x0, y0, z0, x1, y1, y2);
}

void PhysExt_GetDebugString(char** str, i32* length)
{
    *str = g_debugString;
    *length = g_debugStringSize;
}

i32 PhysExt_QueryHitscan()
{
    return 0;
}

i32 PhysExt_QueryVolume()
{
    return 0;
}

Vec3 PhysExt_DebugGetPosition()
{
    return g_testPos;
}

#if 0
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

}
#endif

/////////////////////////////////////////////////////////////
// Lifetime
/////////////////////////////////////////////////////////////

// If the provided command buffer is NULL or the size is zero, fail
// TODO: Could give the option to call function pointers for input/output
// instead of using buffers.
void PhysExt_Init(
    void* ptrCommandBuffer,
    u32 commandBufferSize,
    void* ptrEventBuffer,
    u32 eventBufferSize,
    PhysErrorHandler errorHandler
    )
{
    if (
        commandBufferSize == 0
        || ptrCommandBuffer == NULL
        || eventBufferSize == 0
        || ptrEventBuffer == NULL
        )
    {
        COM_ASSERT(0, "Physics wrapper innit failed");
    }

	printf("PHYS INIT\n");

    g_errorHandler = errorHandler;

    COM_ZeroMemory((u8*)ptrCommandBuffer, commandBufferSize);
    COM_ZeroMemory((u8*)ptrEventBuffer, eventBufferSize);

    g_input = {};
    g_input.ptrStart = (u8*)ptrCommandBuffer;
    g_input.ptrEnd = g_input.ptrStart;
    g_input.ptrWrite = g_input.ptrStart;
    g_input.capacity = commandBufferSize;

    g_output = {};
    g_output.ptrStart = (u8*)ptrEventBuffer;
    g_output.ptrEnd = g_output.ptrStart;
    g_output.ptrWrite = g_output.ptrStart;
    g_output.capacity = eventBufferSize;
    

    //g_physTest = {};
    g_world = {};
	//g_world.verbose = 1;
    g_world.bodies.items = g_bodies;
    g_world.bodies.capacity = MAX_PHYS_BODIES;
    g_world.overlapPairs = g_overlapPairs;
    g_world.numOverlaps = MAX_PHYS_OVERLAPS;

    g_world.broadphase = new btDbvtBroadphase();

    g_world.collisionConfiguration = new btDefaultCollisionConfiguration();
    g_world.dispatcher = new btCollisionDispatcher(g_world.collisionConfiguration);

    g_world.solver = new btSequentialImpulseConstraintSolver();

    g_world.dynamicsWorld = new btDiscreteDynamicsWorld(
        g_world.dispatcher,
        g_world.broadphase,
        g_world.solver,
        g_world.collisionConfiguration);

    g_world.dynamicsWorld->setGravity(btVector3(0, -20, 0));

	g_world.dynamicsWorld->setInternalTickCallback(Phys_PreSolveCallback, 0, true);
    g_world.dynamicsWorld->setInternalTickCallback(Phys_PostSolveCallback, 0, false);
    
    //Phys_CreateTestScene(&g_world);
}

void PhysExt_ClearWorld()
{
    if (g_world.verbose)
    {
        printf("PHYS Clear world\n");
    }
	Phys_ClearOverlapPairs(&g_world);
	Buf_Clear(&g_input);
	Buf_Clear(&g_output);
    for (int i = 0; i < g_world.bodies.capacity; ++i)
    {
        Phys_FreeHandle(&g_world, &g_world.bodies.items[i]);
    }
}

void PhysExt_Shutdown()
{
    PhysExt_ClearWorld();
    // Get order right or it will cause an access violation
	delete g_world.dynamicsWorld;
	delete g_world.solver;
	delete g_world.dispatcher;
	delete g_world.collisionConfiguration;
    delete g_world.broadphase;
}

ByteBuffer PhysExt_Step(f32 deltaTime)
{
    //MemoryBlock eventBuffer = {};

    
    
    
    
    
    
    //eventBuffer.ptrMemory = g_output.ptrStart;
    //eventBuffer.size = g_output.capacity;

    // TODO: Lot of ickyness here
    // Internal functions should not reference global buffers, but receive buffers via
    // params, so that locations of 'current' buffers can be changed in this function
    Phys_LockCommandBuffer(&g_input);
    u32 reading = g_input.ptrWrite - g_input.ptrStart;
    if (reading > g_world.debug.inputPeakBytes)
    {
        g_world.debug.inputPeakBytes = reading;
    }
	if (g_world.verbose)
	{
		printf("PHYS Reading %d bytes\n", reading);
	}

    // Reset output
    g_output.ptrWrite = g_output.ptrStart;
    g_output.ptrEnd = g_output.ptrStart;

    // So commands both read and write... Is this going to work?
    Phys_ReadCommands(&g_world, &g_output);

    Phys_StepWorld(&g_world, deltaTime);
    u32 written = g_output.ptrWrite - g_output.ptrStart;
    if (written > g_world.debug.outputPeakBytes)
    {
        g_world.debug.outputPeakBytes = written;
    }
	if (g_world.verbose)
	{
		printf("PHYS Wrote %d bytes\n", written);
	}

    // TODO: Should only write the debug string if is asked for!
    Phys_WriteDebugOutput(&g_world);
    ByteBuffer buf = g_output;
    return buf;
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
