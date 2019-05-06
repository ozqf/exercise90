#pragma once

#include "physics_module.cpp"


////////////////////////////////////////////////////////////////////////////////////////////
// OVERLAP RECORDS
////////////////////////////////////////////////////////////////////////////////////////////

internal void Phys_ClearOverlapPairs(ZBulletWorld* world)
{
	for (int i = 0; i < world->numOverlaps; ++i)
	{
		PhysOverlapPair* p = &world->overlapPairs[i];
		*p = {};
	}
}

internal i32 Phys_FindOverlapPair(ZBulletWorld *world, i32 a, i32 b)
{
    for (int i = 0; i < world->numOverlaps; ++i)
    {
        PhysOverlapPair* p = &world->overlapPairs[i];
        // order of indices doesn't matter
        if (
            p->isActive &&
            ((p->a.internalId == a && p->b.internalId == b) ||
            (p->a.internalId == b && p->b.internalId == a)))
        {
            return i;
        }
    }
    return -1;
}

internal i32 Phys_AddOverlapPair(ZBulletWorld *world, i32 a, i32 b, u32 frame)
{
    for (int i = 0; i < PHYS_MAX_OVERLAPS; ++i)
    {
        PhysOverlapPair* p = &world->overlapPairs[i];
        if (p->isActive) { continue; }

        PhysBodyHandle* handleA = Phys_GetHandleById(&world->bodies, a);
		PhysBodyHandle* handleB = Phys_GetHandleById(&world->bodies, b);
		if (!handleA || !handleB)
		{
			Phys_Error("Add overlap pair failed: Null handle\n");
		}
        
        *p = {};
        p->isActive = 1;
        p->startFrame = frame;
        p->latestFrame = frame;

        p->a.internalId = a;
        p->a.externalId = handleA->externalId;
        p->a.shapeTag = handleA->def.tag;

        p->b.internalId = b;
        p->b.externalId = handleB->externalId;
        p->b.shapeTag = handleB->def.tag;

        // report begin/end touch?
        if ((handleA->def.flags & ZCOLLIDER_FLAG_INTERESTING) &&
            (handleB->def.flags & ZCOLLIDER_FLAG_INTERESTING))
        {
            p->reportLevel = 1;
        }

        // report every frame
        if ((handleA->def.flags & ZCOLLIDER_FLAG_VERY_INTERESTING) ||
            (handleB->def.flags & ZCOLLIDER_FLAG_VERY_INTERESTING))
        {
            p->reportLevel = 2;
        }

        return i;
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CALLBACKS
////////////////////////////////////////////////////////////////////////////////////////////
internal void Phys_PreSolveCallback(btDynamicsWorld *dynWorld, btScalar timeStep)
{
    ZBulletWorld* world = (ZBulletWorld*)dynWorld->getWorldUserInfo();
	COM_ASSERT(world != NULL, "World is null")
    world->debug.preSolves++;
}

internal void Phys_WriteCollisionEvent(PhysEv_Collision* ev, PhysOverlapPair* pair)
{
	ev->a.externalId = pair->a.externalId;
	ev->a.shapeTag = pair->a.shapeTag;
	ev->b.externalId = pair->b.externalId;
	ev->b.shapeTag = pair->b.shapeTag;
}

/*
> Iterate bt's overlaps.
> Search current overlap pairs array for that overlap.
    > If not found, add
    > If found, set postSolveFrame.
> Afterward, iterate overlaps.
    > if startFrame == this frame, write 'collisionStarted' cmd
    > if latestFrame != this frame, write 'collision ended' cmd and wipe overlap record
*/
internal void Phys_PostSolveCallback(btDynamicsWorld *dynWorld, btScalar timeStep)
{
    ZBulletWorld* w = (ZBulletWorld*)dynWorld->getWorldUserInfo();
    u32 currentFrame = ++w->debug.postSolves;

    int numManifolds = dynWorld->getDispatcher()->getNumManifolds();
    w->debug.numManifolds = numManifolds;
    for (int i = 0; i < numManifolds; i++)
    {
        // All this spiel is iterating bullet's collisions 
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
                // if you want more info...?
                //const btVector3 &ptA = pt.getPositionWorldOnA();
                //const btVector3 &ptB = pt.getPositionWorldOnB();
                //const btVector3 &normalOnB = pt.m_normalWorldOnB;
            }
        }

        if (areTouching)
        {
            // Cool we have stuff to do
            i32 pairIndex = Phys_FindOverlapPair(w, indexA, indexB);
            if (pairIndex >= 0)
            {
                w->overlapPairs[pairIndex].latestFrame = currentFrame;
                continue;
            }
            else
            {
                pairIndex = Phys_AddOverlapPair(w, indexA, indexB, currentFrame);
                COM_ASSERT(pairIndex != -1, "No capacity for overlap pair");
            }
        }
    }

    // Output start/end events
    ByteBuffer* buf = &w->output;
    PhysDataItemHeader h = {};
    PhysEv_Collision ev = {};
    
    for (int i = 0; i < PHYS_MAX_OVERLAPS; ++i)
    {
        PhysOverlapPair* pair = &w->overlapPairs[i];
        if (!pair->isActive) { continue; }
        if (pair->reportLevel == 0)
		{
			// Even if not reporting, must check to set inactive.
			if (pair->latestFrame != currentFrame)
			{
				pair->isActive = 0;
			}
			continue;
		}
        if (pair->startFrame == currentFrame)
        {
            // collision began
            h.size = sizeof(PhysEv_Collision);
            h.type = OverlapStarted;
			Phys_WriteCollisionEvent(&ev, pair);
            buf->ptrWrite += COM_COPY_STRUCT(&h, buf->ptrWrite, PhysDataItemHeader);
            buf->ptrWrite += COM_COPY_STRUCT(&ev, buf->ptrWrite, PhysEv_Collision);
            printf("** Collision %d vs %d start\n", pair->a.externalId, pair->b.externalId);
        }
        else if (pair->latestFrame != currentFrame)
        {
            // collision ended
            h.size = sizeof(PhysEv_Collision);
            h.type = OverlapEnded;
			Phys_WriteCollisionEvent(&ev, pair);
            buf->ptrWrite += COM_COPY_STRUCT(&h, buf->ptrWrite, PhysDataItemHeader);
            buf->ptrWrite += COM_COPY_STRUCT(&ev, buf->ptrWrite, PhysEv_Collision);
            pair->isActive = 0;
            printf("** Collision %d vs %d end\n", pair->a.externalId, pair->b.externalId);
        }
        else
        {
            if (pair->reportLevel == 2)
            {
                h.size = sizeof(PhysEv_Collision);
                h.type = OverlapInProgress;
			    Phys_WriteCollisionEvent(&ev, pair);
                buf->ptrWrite += COM_COPY_STRUCT(&h, buf->ptrWrite, PhysDataItemHeader);
                buf->ptrWrite += COM_COPY_STRUCT(&ev, buf->ptrWrite, PhysEv_Collision);
            }
        }
    }
    buf->ptrEnd = buf->ptrWrite;

    //printf("Post solve overlaps %d\n", w->numOverlaps);
}
