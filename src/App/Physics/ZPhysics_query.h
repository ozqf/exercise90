#pragma once

#include "ZPhysics_module.cpp"

// Return bytes written
inline i32 Phys_ExecRaycast(ZBulletWorld *world, PhysCmd_Raycast* cmd, ByteBuffer* buf)
{
    // prepare buffer header. cannot write it until the end when results are know.
    // record write pos and step, write later
    // record start position to calculate bytes written by the end
    u8* ptrStart = buf->ptrWrite;
    u8* ptrHeaderWrite = buf->ptrWrite;
    buf->ptrWrite += sizeof(PhysDataItemHeader) + sizeof(PhysEv_RaycastResult);

    // Prepare results header
    PhysEv_RaycastResult result = {};
    result.queryId = cmd->id;
    result.start[0] = cmd->start[0];
    result.start[1] = cmd->start[1];
    result.start[2] = cmd->start[2];

    result.end[0] = cmd->end[0];
    result.end[1] = cmd->end[1];
    result.end[2] = cmd->end[2];

    result.numHits = 0;

    btVector3 start(result.start[0], result.start[2], result.start[2]);
    btVector3 end(cmd->end[0], cmd->end[1], cmd->end[2]);
    btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);

    world->dynamicsWorld->rayTest(start, end, rayCallback);

    if (rayCallback.hasHit())
    {
        result.numHits++;
        PhysRayHit hit = {};
        btVector3 hitPoint = rayCallback.m_hitPointWorld;
        btVector3 normal = rayCallback.m_hitNormalWorld;
        hit.worldPos[0] = hitPoint.getX();
        hit.worldPos[1] = hitPoint.getY();
        hit.worldPos[2] = hitPoint.getZ();

        hit.normal[0] = normal.getX();
        hit.normal[1] = normal.getY();
        hit.normal[2] = normal.getZ();
        
        // Need to figure this out...
        // kinda important to know
        //hit.shapeId = 
        buf->ptrWrite += COM_COPY_STRUCT(&hit, buf->ptrWrite, PhysRayHit);
    }

    PhysDataItemHeader h = {};
    h.type = RaycastResult;
    h.size = sizeof(PhysEv_RaycastResult) + (sizeof(PhysRayHit) * result.numHits);

    ptrHeaderWrite += COM_COPY_STRUCT(&h, ptrHeaderWrite, PhysDataItemHeader);
    ptrHeaderWrite += COM_COPY_STRUCT(&result, ptrHeaderWrite, PhysEv_RaycastResult);

    return (buf->ptrWrite - ptrStart);
}

i32 PhysCmd_RayTest(
    ZBulletWorld *world,
    f32 x0, f32 y0, f32 z0,
    f32 x1, f32 y1, f32 z1)
{
    btVector3 start(x0, y0, z0);
    btVector3 end(x1, y1, z1);
    btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);

    world->dynamicsWorld->rayTest(start, end, rayCallback);

    if (rayCallback.hasHit())
    {
        btVector3 dir(x1 - x0, y1 - y0, z1 - z0);
        dir.normalize();
        const btCollisionObject* obj = rayCallback.m_collisionObject;
        #if 0
        if (!obj->isStaticOrKinematicObject())
        {
            //const btRigidBody *body = btRigidBody::upcast(obj);
            btRigidBody *foo = const_cast<btRigidBody *>(btRigidBody::upcast(obj));
            if (foo != 0)
            {
                Phys_SetBodyVelocity(foo, dir.getX() * 5, dir.getY() * 5, dir.getZ() * 5);
            }
        }
        #endif
    }

    return world->nextQueryId++;
}

inline u8 PhysCmd_GroundTest(
    ZBulletWorld *world,
    f32 x0, f32 y0, f32 z0,
    PhysEv_RaycastDebug* ev)
{
    f32 halfHeight = (1.85f / 2.0f);
    f32 y1 = y0 -  halfHeight - 0.2f;
    btVector3 start(x0, y0, z0);
    btVector3 end(x0, y1, z0);
    if (ev != NULL)
    {
        ev->a[0] = x0;
        ev->a[1] = y0;
        ev->a[2] = z0;

        ev->b[0] = x0;
        ev->b[1] = y1;
        ev->b[2] = z0;
    }
    btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);

    world->dynamicsWorld->rayTest(start, end, rayCallback);

    if (rayCallback.hasHit())
    {
        if (ev != NULL)
        {
            ev->colour[0] = 1;
            ev->colour[1] = 0;
            ev->colour[2] = 0;
        }
        return 1; 
        #if 0
        btVector3 dir(x1 - x0, y1 - y0, z1 - z0);
        dir.normalize();
        const btCollisionObject* obj = rayCallback.m_collisionObject;
        #endif
    }
    if (ev != NULL)
        {
            ev->colour[0] = 0;
            ev->colour[1] = 1;
            ev->colour[2] = 0;
        }
    return 0;
}

