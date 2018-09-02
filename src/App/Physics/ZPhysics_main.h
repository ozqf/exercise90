#pragma once

#include "ZPhysics_module.cpp"

////////////////////////////////////////////////////////////////////////////////////////////
// MANAGE HANDLES
////////////////////////////////////////////////////////////////////////////////////////////
PhysBodyHandle *Phys_GetFreeBodyHandle(PhysBodyList *list)
{

    for (i32 i = 0; i < list->capacity; ++i)
    {
        PhysBodyHandle *handle = &list->items[i];
        if (handle->inUse == FALSE)
        {
            handle->id = i;
            handle->inUse = TRUE;
            return handle;
        }
    }
    return NULL;
}

PhysBodyHandle *Phys_GetHandleById(PhysBodyList *list, i32 queryId)
{
    Assert(queryId >= 0);
    Assert(queryId < list->capacity);
    // Handles that have not been 'issued' have no business being found!
    Assert(list->items[queryId].inUse);
    return &list->items[queryId];
}

// Delete all objects on this handle
void Phys_FreeHandle(ZBulletWorld *world, PhysBodyHandle *handle)
{
    // TODO: This is almost certainly NOT how to free stuff in bullet
    // !find proper example!
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
void Phys_RecycleHandle(ZBulletWorld *world, PhysBodyHandle *handle)
{
    handle->inUse = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CREATE SHAPES
////////////////////////////////////////////////////////////////////////////////////////////

internal i32 Phys_FindOverlapPair(ZBulletWorld *world, i32 a, i32 b)
{
    for (int i = 0; i < world->numOverlaps; ++i)
    {
        // order of indices doesn't matter
        if (
            (world->overlapPairs[i].indexA == a && world->overlapPairs[i].indexB == b) ||
            (world->overlapPairs[i].indexA == b && world->overlapPairs[i].indexB == a))
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
    ZBulletWorld *w = &g_world;
    ++w->debug.postSolves;

    // Reset overlap list and rebuild
    w->numOverlaps = 0;

    int numManifolds = dynWorld->getDispatcher()->getNumManifolds();
    g_world.debug.numManifolds = numManifolds;
    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold *contactManifold = dynWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject *obA = contactManifold->getBody0();
        const btCollisionObject *obB = contactManifold->getBody1();

        i32 indexA = obA->getUserIndex();
        i32 indexB = obB->getUserIndex();

        int numContacts = contactManifold->getNumContacts();
        u8 areTouching = 0;

        for (int j = 0; j < numContacts; j++)
        {
            btManifoldPoint &pt = contactManifold->getContactPoint(j);
            //if (pt.getDistance() < 0.05f)
            if (pt.getDistance() < F32_EPSILON)
            {
                areTouching = 1;
                const btVector3 &ptA = pt.getPositionWorldOnA();
                const btVector3 &ptB = pt.getPositionWorldOnB();
                const btVector3 &normalOnB = pt.m_normalWorldOnB;
            }
        }

        if (areTouching)
        {
            if (Phys_FindOverlapPair(w, indexA, indexB) >= 0)
            {
                continue;
            }

            Assert(w->numOverlaps < MAX_PHYS_OVERLAPS);
            w->overlapPairs[w->numOverlaps].indexA = indexA;
            w->overlapPairs[w->numOverlaps].indexB = indexB;
            w->numOverlaps++;
        }
    }
}

void Phys_NeverCall()
{
    ILLEGAL_CODE_PATH
    Phys_CreateBulletBox(NULL, NULL, NULL);
    PhysExec_CreateShape(NULL, NULL);
    Phys_RecycleHandle(NULL, NULL);
    Phys_ReadCommands(NULL, NULL);
}

inline void PhysExec_SetBodyPosition(btRigidBody *body, f32 x, f32 y, f32 z)
{
    btTransform t;
    body->getMotionState()->getWorldTransform(t);
    btVector3 newPosition = t.getOrigin();
    newPosition.setX(-newPosition.getX() + x);
    newPosition.setY(-newPosition.getY() + y);
    newPosition.setZ(-newPosition.getZ() + z);
    body->translate(newPosition);
}

inline void Phys_SetBodyVelocity(btRigidBody *body, f32 vx, f32 vy, f32 vz)
{
    body->activate(true);
    btVector3 newVelocity;
    newVelocity.setX(vx);
    newVelocity.setY(vy);
    newVelocity.setZ(vz);
    body->setLinearVelocity(newVelocity);
}

void PhysExec_TeleportShape(ZBulletWorld *world, PhysCmd_Teleport *cmd)
{
    PhysBodyHandle *handle = Phys_GetHandleById(&world->bodies, cmd->shapeId);
    if (handle == NULL)
    {
        return;
    }
    handle->rigidBody->activate(true);
    PhysExec_SetBodyPosition(
        handle->rigidBody,
        cmd->pos[0],
        cmd->pos[1],
        cmd->pos[2]
    );
#if 0
    btTansform t;
    handle->rigidBody->getMotionState()->getWorldTransform(t);
    btVector3 newPosition = t.getOrigin();
    newPosition.setX(-newPosition.getX() + cmd->pos[0]);
    newPosition.setY(-newPosition.getY() + cmd->pos[1]);
    newPosition.setZ(-newPosition.getZ() + cmd->pos[2]);
    /*handle->rigidBody->translate(btVector3(cmd->pos[0], cmd->pos[1], cmd->pos[2]));*/
    handle->rigidBody->translate(newPosition);
    //btMotionState state = handle->rigidBody->getMotionState();
#endif
}

void PhysExec_ChangeVelocity(ZBulletWorld *world, PhysCmd_VelocityChange *cmd)
{
    PhysBodyHandle *handle = Phys_GetHandleById(&world->bodies, cmd->shapeId);
    if (handle == NULL)
    {
        return;
    }
    if (handle->def.flags & ZCOLLIDER_FLAG_STATIC)
    {
        printf("PHYS Attempting to Change velocity of static body %d\n", cmd->shapeId);
        return;
    }
    Phys_SetBodyVelocity(handle->rigidBody, cmd->vel[0], cmd->vel[1], cmd->vel[2]);
    /*handle->rigidBody->activate(true);
	btVector3 newVelocity;
	newVelocity.setX(cmd->vel[0]);
	newVelocity.setY(cmd->vel[1]);
	newVelocity.setZ(cmd->vel[2]);
	handle->rigidBody->setLinearVelocity(newVelocity);*/
}

void PhysExec_SetState(ZBulletWorld* world, PhysCmd_State *cmd)
{
    PhysBodyHandle *handle = Phys_GetHandleById(&world->bodies, cmd->shapeId);
    if (handle == NULL)
    {
        return;
    }
    handle->rigidBody->activate(true);
    btTransform t;
}

////////////////////////////////////////////////////////////////////////////////////////////
// LOOP
////////////////////////////////////////////////////////////////////////////////////////////
void Phys_LockCommandBuffer(ByteBuffer *buffer)
{
    *buffer->ptrWrite = 0;
    buffer->ptrEnd = buffer->ptrWrite;
}

void Phys_ReadCommands(ZBulletWorld *world, ByteBuffer* output)
{
    //Phys_TickCallback(g_world.dynamicsWorld, 0.016f);
    ByteBuffer *buffer = &g_cmdBuf;
    u8 *ptrRead = buffer->ptrStart;
    while (*ptrRead != NULL && ptrRead < buffer->ptrEnd)
    {
        u8 cmdType = COM_ReadByte(&ptrRead);
        switch (cmdType)
        {
            case Teleport:
            {
                PhysCmd_Teleport cmd = {};
                ptrRead += COM_COPY_STRUCT(ptrRead, &cmd, PhysCmd_Teleport);
                PhysExec_TeleportShape(world, &cmd);
            } break;

            case SetVelocity:
            {
                PhysCmd_VelocityChange cmd = {};
                ptrRead += COM_COPY_STRUCT(ptrRead, &cmd, PhysCmd_VelocityChange);
                PhysExec_ChangeVelocity(world, &cmd);
            } break;

            case SetState:
            {
                PhysCmd_State cmd = {};
                ptrRead += COM_COPY_STRUCT(ptrRead, &cmd, PhysCmd_State);
                PhysExec_SetState(world, &cmd);
            } break;

            case Create:
            {
                ZShapeDef def = {};
                ptrRead += COM_COPY_STRUCT(ptrRead, &def, ZShapeDef);
                PhysExec_CreateShape(world, &def);
            } break;

            case Remove:
            {
                i32 shapeId = COM_ReadI32(&ptrRead);
                PhysBodyHandle* h = Phys_GetHandleById(&g_world.bodies, shapeId);
                if (h == NULL)
                {
                    printf("PHYS: Not shape %d to remove!\n", shapeId);
                    break;
                }
                else
                {
                    printf("PHYS Removed shape %d\n", shapeId);
                    Phys_FreeHandle(&g_world, h);
                }
            } break;

            // TODO:
            // So how is this going to work exactly...?
            #if 0
            case Raycast:
            {
                PhysCmd_Raycast cmd = {};
                ptrRead += COM_COPY_STRUCT(ptrRead, &cmd, PhysCmd_Raycast);
                // needs output buffer for results...
                Phys_ExecRaycast(world, &cmd, output);
            }
            break
            #endif;

            default:
            {
                ILLEGAL_CODE_PATH
            } break;
        }
    }
    // Clear
    buffer->ptrEnd = buffer->ptrStart;
    COM_WriteByte(0, buffer->ptrStart);
    // reset buffer write position
    buffer->ptrWrite = buffer->ptrStart;
}

internal void Phys_StepWorld(ZBulletWorld *world, MemoryBlock *eventBuffer, f32 deltaTime)
{
    ++world->debug.stepCount;
    world->dynamicsWorld->stepSimulation(deltaTime, 10, deltaTime);

    u8 *writePosition = (u8 *)eventBuffer->ptrMemory;
	u8 *endPosition = writePosition + eventBuffer->size;
    i32 len = world->bodies.capacity;
	i32 updatesWritten = 0;
	i32 unusedSkipped = 0;
	i32 inactiveSkipped = 0;
    for (int i = 0; i < len; ++i)
    {
        PhysBodyHandle *h = &world->bodies.items[i];
        // Check that a rigidbody is present and that the handle is in use.
        // Otherwise it might be a rigidbody await recylcing
        if (h->inUse == FALSE || h->rigidBody == NULL)
        {
			unusedSkipped++;
            continue;
        }
        if (!h->rigidBody->getActivationState())
        {
			inactiveSkipped++;
            continue;
        }
        Assert(h->motionState != NULL);
        i32 requiredSize = sizeof(PhysDataItemHeader) + sizeof(PhysEV_TransformUpdate);
		Assert((writePosition +  requiredSize < endPosition));

		updatesWritten++;

        
        PhysDataItemHeader dataHeader = {};
        dataHeader.type = TransformUpdate;
        // TODO: If updates vary in size in the future (and they are big,
        // what with containing an entire matrix and all) this will have to be set
        // AFTER the update is written to the buffer
        dataHeader.size = sizeof(TransformUpdate);
        writePosition += COM_COPY_STRUCT(&dataHeader, writePosition, PhysDataItemHeader);

        PhysEV_TransformUpdate ev = {};
        ev.ownerId = h->ownerId;
        ev.ownerIteration = h->ownerIteration;
        ev.tag = h->def.tag;
        
        btTransform t;
        h->rigidBody->getMotionState()->getWorldTransform(t);

        btScalar openglM[16];
        t.getOpenGLMatrix(openglM);
        for (i32 j = 0; j < 16; ++j)
        {
            ev.matrix[j] = openglM[j];
        }

        btVector3 vel = h->rigidBody->getLinearVelocity();
        ev.vel[0] = vel.getX();
        ev.vel[1] = vel.getY();
        ev.vel[2] = vel.getZ();

        PhysEv_RaycastDebug rayEv = {};
        
        if ((
            h->def.flags & ZCOLLIDER_FLAG_NO_ROTATION) ||
            h->def.flags & ZCOLLIDER_FLAG_GROUNDCHECK)
        {
            // TODO: Assuming handle is for a box and halfsize is valid!
            u8 val = PhysCmd_GroundTest(
                world, openglM[M4x4_W0], openglM[M4x4_W1], openglM[M4x4_W2],
                h->def.data.box.halfSize[1],
                &rayEv);
            if (val)
            {
                ev.flags |= PHYS_EV_FLAG_GROUNDED;
            }
        }

        writePosition += COM_COPY_STRUCT(&ev, writePosition, PhysEV_TransformUpdate);

        // Write ground check debug
        dataHeader.type = RaycastDebug;
        dataHeader.size = sizeof(PhysEv_RaycastDebug);
        writePosition += COM_COPY_STRUCT(&dataHeader, writePosition, PhysDataItemHeader);

        writePosition += COM_COPY_STRUCT(&rayEv, writePosition, PhysEv_RaycastDebug);
    }

    // Mark end of buffer
    *writePosition = 0;

    /*char buf[128];
        sprintf_s(buf, 128, "Sphere pos: %.2f, %.2f, %.2f\n", pos.getX(), pos.getY(), pos.getZ());
        OutputDebugString(buf);*/
}

void OldDefunctWriteUpdateCode()
{

        /*btVector3 pos = t.getOrigin();
		tUpdate.matrix[12] = pos.x();
		tUpdate.matrix[13] = pos.y();
		tUpdate.matrix[14] = pos.z();*/

#if 0 // Copy rotation via calculating euler angles in opengl matrix
		t.getOpenGLMatrix(openglM);
		
		f32 fAngZ = atan2f(openglM[1], openglM[5]);
		f32 fAngY = atan2f(openglM[8], openglM[10]);
		f32 fAngX = -asinf(openglM[9]);
		tUpdate.rot[0] = fAngX;// *RAD2DEG;
		tUpdate.rot[1] = fAngY;// * RAD2DEG;
		tUpdate.rot[2] = fAngZ;// * RAD2DEG;
#endif

#if 0 // Rotation copy to matrix via euler angles into bottom row
		btMatrix3x3& rot = t.getBasis();
		btScalar yaw, pitch, roll;
		rot.getEulerYPR(yaw, pitch, roll);
		tUpdate.matrix[3] = roll;
		tUpdate.matrix[7] = pitch;
		tUpdate.matrix[11] = yaw;
#endif

#if 0 // Rotation copy to matrix directly
		btMatrix3x3& rot = t.getBasis();
		btVector3 xAxis = rot.getColumn(0);
		btVector3 yAxis = rot.getColumn(1);
		btVector3 zAxis = rot.getColumn(2);

		tUpdate.matrix[0] = xAxis.x();
		tUpdate.matrix[1] = xAxis.y();
		tUpdate.matrix[2] = xAxis.z();

		tUpdate.matrix[4] = yAxis.x();
		tUpdate.matrix[5] = yAxis.y();
		tUpdate.matrix[6] = yAxis.z();

		tUpdate.matrix[8] = zAxis.x();
		tUpdate.matrix[9] = zAxis.y();
		tUpdate.matrix[10] = zAxis.z();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG
////////////////////////////////////////////////////////////////////////////////////////////
internal void Phys_WriteDebugOutput(ZBulletWorld *world)
{
    char *ptr = g_debugString;
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
                        world->numOverlaps);
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
