#pragma once

#include "game.h"

/////////////////////////////////////////////////////////////////////////////
// PHYSICS OUTPUT
/////////////////////////////////////////////////////////////////////////////
#if 0
internal void Game_SpawnHitRing(Vec3 pos, EntId source, EntId victim)
{
    Cmd_SpawnHudItem cmd = {};
    cmd.pos.x = pos.x;
    cmd.pos.y = pos.y;
    cmd.pos.z = pos.z;
    cmd.source = source;
    cmd.victim = victim;
    APP_WRITE_CMD(0, cmd);
}
#endif

internal void Game_EntVsEntCollision(
    GameScene* gs,
    GameTime* time,
    u32 id_A,
    u32 id_B,
    i32 collisionState)
{
    Ent* a = Ent_GetEntityByIdValue(&gs->entList, id_A);
    Ent* b = Ent_GetEntityByIdValue(&gs->entList, id_B);
	
	if (a == NULL || b == NULL)
	{
		if (collisionState == 3)
		{
			// entities could have been removed before an end collision
			// event is called, so this can be expected
			return;
		}
		else
		{
			// Appears to be occurring on clients.
			// Warn about it and see what happens!
			EntId aId = {};
			aId.value = id_A;
			EntId bId = {};
			bId.value = id_B;
			if (a == NULL)
			{
				printf("PHYS Report Col state %d: Entity %d/%d is null!\n",
					collisionState, aId.iteration, aId.index);
			}
			if (b == NULL)
			{
				printf("PHYS Report Col state %d: Entity %d/%d is null!\n",
					collisionState, aId.iteration, aId.index);
			}
			return;
		}
	}
	
    // Sensors
    EC_Sensor* sensorA = EC_FindSensor(gs, a);
    EC_Sensor* sensorB = EC_FindSensor(gs, b);
    if (sensorA)
    {
        // printf("SENSOR HIT: %d/%d vs %d/%d\n", 
            // a->entId.iteration, a->entId.index,
            // b->entId.iteration, b->entId.index
        // );
        EC_SensorHandleHit(gs, time, sensorA, a, b, collisionState);
    }
    if (sensorB)
    {
        // printf("SENSOR HIT: %d/%d vs %d/%d\n", 
            // b->entId.iteration, b->entId.index,
            // a->entId.iteration, a->entId.index
        // );
        EC_SensorHandleHit(gs, time, sensorB, b, a, collisionState);
    }
}

void Game_HandleEntityUpdate(GameScene *gs, PhysEV_TransformUpdate *ev)
{
    if (ev->tag == 0)
    {
        EntId entId = {};
        entId.value = ev->ownerId;
        EC_Transform* ecTrans = EC_FindTransform(gs, &entId);
        if (ecTrans == NULL)
        {
            return;
        }
        M4x4* updateM = (M4x4*)&ev->matrix;
    	//f32 magX = Vec3_Magnitudef(updateM->x0, updateM->x1, updateM->x2);
    	//f32 magY = Vec3_Magnitudef(updateM->y0, updateM->y1, updateM->y2);
    	//f32 magZ = Vec3_Magnitudef(updateM->z0, updateM->z1, updateM->z2);
    #if 0
    	//ent->transform.scale = { 1, 1, 1 };
    	ent->transform.pos.x = updateM->posX;
    	ent->transform.pos.y = updateM->posY;
    	ent->transform.pos.z = updateM->posZ;
    	Transform_ClearRotation(&ent->transform);
    	Transform_RotateX(&ent->transform, ev->rot[0]);
    	Transform_RotateY(&ent->transform, ev->rot[1]);
    	Transform_RotateZ(&ent->transform, ev->rot[2]);
    	/*Transform_RotateX(&ent->transform, updateM->xAxisW);
    	Transform_RotateY(&ent->transform, updateM->yAxisW);
    	Transform_RotateZ(&ent->transform, updateM->zAxisW);*/
    #endif
    #if 1
        Transform_FromM4x4(&ecTrans->t, updateM);
    #endif
        EC_Collider* col = EC_FindCollider(gs, &entId);
        Assert(col != NULL);
        col->state.velocity.x = ev->vel[0]; 
        col->state.velocity.y = ev->vel[1];
        col->state.velocity.z = ev->vel[2];
        col->isGrounded = (ev->flags & PHYS_EV_FLAG_GROUNDED);
    }
    else
    {
        M4x4* updateM = (M4x4*)&ev->matrix;
        // object is a local ent
        LocalEnt* e = Game_GetLocalEntByIndex(ev->ownerId);
        e->pos.x = updateM->posX;
        e->pos.y = updateM->posY;
        e->pos.z = updateM->posZ;
		Vec3 rot = M4x4_GetEulerAnglesDegrees(ev->matrix);
		e->rotationDegrees.x = rot.x;
		e->rotationDegrees.y = rot.y;
		e->rotationDegrees.z = rot.z;
    }
    
}

/////////////////////////////////////////////////////////////////////////////
// STEP PHYSICS
/////////////////////////////////////////////////////////////////////////////
#define MAX_ALLOWED_PHYSICS_STEP 0.0334f
void Game_StepPhysics(GameScene* gs, GameTime* time)
{
    
    // Force physics step to always be no lower than 30fps
    // TODO: Force game update and physics step to specific, locked framerate
    /////////////////////////////////////////////////////////////////////////////
    // STEP PHYSICS
    f32 dt = time->deltaTime;
    if (dt > MAX_ALLOWED_PHYSICS_STEP)
    {
        dt = MAX_ALLOWED_PHYSICS_STEP;
    }

    ByteBuffer eventBuffer = PhysExt_Step(dt);
    
    //i32 ptrOffset = 0;
    u8 reading = 1;
	i32 eventsProcessed = 0;

    //u8* readPos = (u8*)eventBuffer.ptrMemory;
    //u8* end = (u8*)((u8*)eventBuffer.ptrMemory + eventBuffer.size);

    u8* readPos = eventBuffer.ptrStart;
    u8* end = eventBuffer.ptrEnd;
    while (readPos < end)
    {
        PhysDataItemHeader h;
        readPos += COM_COPY_STRUCT(readPos, &h, PhysDataItemHeader);
        u8* copyPos = readPos;
        readPos += h.size;
        switch (h.type)
        {
            case TransformUpdate:
            {
                PhysEV_TransformUpdate tUpdate = {};
                COM_COPY_STRUCT(copyPos, &tUpdate, PhysEV_TransformUpdate);
                Game_HandleEntityUpdate(gs, &tUpdate);
		    	eventsProcessed++;
            } break;

            case RaycastDebug:
            {
                PhysEv_RaycastDebug ray = {};
                COM_COPY_STRUCT(copyPos, &ray, PhysEv_RaycastDebug);
                RendObj_SetAsLine(
                    &g_debugLine,
                    ray.a[0], ray.a[1], ray.a[2],
                    ray.b[0], ray.b[1], ray.b[2],
                    ray.colour[0], ray.colour[1], ray.colour[2],
                    ray.colour[0], ray.colour[1], ray.colour[2]
                
                );
                // printf("Draw Raycast %.2f %.2f %.2f to %.2f %.2f %.2f\n",
                //     ray.a[0], ray.a[1], ray.a[2],
                //     ray.b[0], ray.b[1], ray.b[2]
                // );
                eventsProcessed++;
            } break;

            case OverlapStarted:
            {
                PhysEv_Collision ev = {};
                COM_COPY_STRUCT(copyPos, &ev, PhysEv_Collision);
                if (ev.a.shapeTag == 1 || ev.b.shapeTag == 1) { continue; }
                //printf("Overlap started: %d vs %d\n", ev.a.externalId, ev.b.externalId);
                Game_EntVsEntCollision(gs, time, ev.a.externalId, ev.b.externalId, 1);
            } break;

            case OverlapInProgress:
            {
                PhysEv_Collision ev = {};
                COM_COPY_STRUCT(copyPos, &ev, PhysEv_Collision);
                if (ev.a.shapeTag == 1 || ev.b.shapeTag == 1) { continue; }
                Game_EntVsEntCollision(gs, time, ev.a.externalId, ev.b.externalId, 2);
            } break;

            case OverlapEnded:
            {
                PhysEv_Collision ev = {};
                COM_COPY_STRUCT(copyPos, &ev, PhysEv_Collision);
                if (ev.a.shapeTag == 1 || ev.b.shapeTag == 1) { continue; }
                Game_EntVsEntCollision(gs, time, ev.a.externalId, ev.b.externalId, 3);
            } break;

            case NULL:
            {
                printf("End of buffer\n");
            } break;

            default:
            {
                printf("Unknown physics event type %d: size %d\n", h.type, h.size);
                //readPos = end;
            } break;
        }
    }
}
