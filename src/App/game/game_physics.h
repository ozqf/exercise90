#pragma once

#include "game.h"

inline void Game_HandleEntityUpdate(GameState *gs, PhysEV_TransformUpdate *ev)
{
    EntId entId = {};
    entId.index = ev->ownerId;
    entId.iteration = ev->ownerIteration;
    Ent *ent = Ent_GetEntityById(&gs->entList, &entId);
    if (ent == NULL)
    {
        return;
    }
    M4x4* updateM = (M4x4*)&ev->matrix;
	f32 magX = Vec3_Magnitudef(updateM->x0, updateM->x1, updateM->x2);
	f32 magY = Vec3_Magnitudef(updateM->y0, updateM->y1, updateM->y2);
	f32 magZ = Vec3_Magnitudef(updateM->z0, updateM->z1, updateM->z2);
    // Debugging trap
	// if (ZABS(magX) > 1.1f)
	// {
	// 	int foo = 1;
	// }
	// if (ZABS(magY) > 1.1f)
	// {
	// 	int foo = 1;
	// }
	// if (ZABS(magZ) > 1.1f)
	// {
	// 	int foo = 1;
	// }

    EC_Collider* col = EC_FindCollider(gs, &entId);
    Assert(col != NULL);
    col->state.velocity.x = ev->vel[0]; 
    col->state.velocity.y = ev->vel[1];
    col->state.velocity.z = ev->vel[2];
    col->isGrounded = (ev->flags & PHYS_EV_FLAG_GROUNDED);
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
    Transform_FromM4x4(&ent->transform, updateM);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// STEP PHYSICS
/////////////////////////////////////////////////////////////////////////////
#define MAX_ALLOWED_PHYSICS_STEP 0.0334f
void Game_StepPhysics(GameState* gs, GameTime* time)
{
    
    // Force physics step to always be no lower than 30fps

    /////////////////////////////////////////////////////////////////////////////
    // STEP PHYSICS
    f32 dt = time->deltaTime;
    if (dt > MAX_ALLOWED_PHYSICS_STEP)
    {
        dt = MAX_ALLOWED_PHYSICS_STEP;
    }

    MemoryBlock eventBuffer = Phys_Step(dt);
    
    //i32 ptrOffset = 0;
    u8 reading = 1;
	i32 eventsProcessed = 0;

    u8* readPos = (u8*)eventBuffer.ptrMemory;
    u8* end = (u8*)((u8*)eventBuffer.ptrMemory + eventBuffer.size);
    while (readPos < end)
    {
        //u8 *mem = (u8 *)((u8 *)eventBuffer.ptrMemory + ptrOffset);
        //i32 type = *(i32 *)mem;
        PhysDataItemHeader h;
        readPos += COM_COPY_STRUCT(readPos, &h, PhysDataItemHeader);
        switch (h.type)
        {
            case TransformUpdate:
            {
                PhysEV_TransformUpdate tUpdate = {};
                readPos += COM_COPY_STRUCT(readPos, &tUpdate, PhysEV_TransformUpdate);
                //COM_CopyMemory(mem, (u8 *)&tUpdate, sizeof(PhysEV_TransformUpdate));
                //ptrOffset += sizeof(PhysEV_TransformUpdate);
                Game_HandleEntityUpdate(gs, &tUpdate);
		    	eventsProcessed++;
            } break;

            case RaycastDebug:
            {
                PhysEv_RaycastDebug ray = {};
                readPos += COM_COPY_STRUCT(readPos, (u8*)&ray, PhysEv_RaycastDebug);
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

            default:
            {
                readPos = end;
            } break;
        }
    }
}
