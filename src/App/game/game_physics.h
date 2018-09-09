#pragma once

#include "game.h"

/////////////////////////////////////////////////////////////////////////////
// PHYSICS OUTPUT
/////////////////////////////////////////////////////////////////////////////
internal void Game_EntVsEntCollision(GameState* gs, GameTime* time, u32 id_A, u32 id_B)
{
    //EC_Collider* a = EC_ColliderGetByShapeId(&gs->colliderList, id_A);
    //EC_Collider* b = EC_ColliderGetByShapeId(&gs->colliderList, id_B);

    Ent* a = Ent_GetEntityByIdValue(&gs->entList, id_A);
    Ent* b = Ent_GetEntityByIdValue(&gs->entList, id_B);
    Assert((a != NULL && b != NULL));

    printf("Collision: %d/%d vs %d/%d\n",
        a->entId.iteration, a->entId.index,
        b->entId.iteration, b->entId.index
    );

    //EC_Collider* colA = EC_FindCollider(gs, a);
    //EC_Collider* colB = EC_FindCollider(gs, b);
    //Assert((colA != NULL && colB != NULL));

    //i32 tagA = colA->state.def.tag;
    //i32 tagB = colA->state.def.tag;
    //printf("Tags %d vs %d\n", tagA, tagB);

    // How to figure out what to do with the collision pair?
    // how to determine the situation:
    // a wants to do damage
    // a wants to take damage
    // b wants to do damage
    // b wants to take damage
    
    EC_Health* healthA = EC_FindHealth(gs, a);
    EC_Health* healthB = EC_FindHealth(gs, b);
    EC_AIController* aiA = EC_FindAIController(gs, a);
    EC_AIController* aiB = EC_FindAIController(gs, b);

    // A hits B
    if (aiA != NULL &&
        healthB != NULL &&
        (aiA->state.state == AI_STATE_CHARGING) &&
        Game_AttackIsValid(a->team, b->team)
        )
    {
        printf("A hits B!\n");
        healthB->state.hp -= 10;
        healthB->state.damageThisFrame += 10;
		healthB->state.lastHitFrame = time->gameFrameNumber;
		healthB->state.lastHitSource = a->entId;
    }

    // B hits A
    if (aiB != NULL &&
        healthA != NULL &&
        (aiB->state.state == AI_STATE_CHARGING) &&
        Game_AttackIsValid(b->team, a->team)
        )
    {
        printf("B hits A!\n");
        healthA->state.hp -= 10;
        healthA->state.damageThisFrame += 10;
		healthA->state.lastHitFrame = time->gameFrameNumber;
		healthA->state.lastHitSource = b->entId;
    }
}

inline void Game_HandleEntityUpdate(GameState *gs, PhysEV_TransformUpdate *ev)
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
void Game_StepPhysics(GameState* gs, GameTime* time)
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
                Game_EntVsEntCollision(gs, time, ev.a.externalId, ev.b.externalId);
            } break;

            case OverlapEnded:
            {
                PhysEv_Collision ev = {};
                COM_COPY_STRUCT(copyPos, &ev, PhysEv_Collision);
                if (ev.a.shapeTag == 1 || ev.b.shapeTag == 1) { continue; }
                //printf("Overlap ended: %d vs %d\n", ev.a.externalId, ev.b.externalId);
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
