#pragma once

#include "game.h"

#include "game_intersection_test.cpp"

inline Vec3 Game_RandomSpawnOffset(f32 rangeX, f32 rangeY, f32 rangeZ)
{
    Vec3 v;
    v.x = COM_Randf32() * (rangeX - -rangeX) + -rangeX;
    v.y = COM_Randf32() * (rangeY - -rangeY) + -rangeY;
    v.z = COM_Randf32() * (rangeZ - -rangeZ) + -rangeZ;
    return v;

}

void Game_BuildTestScene(GameState *gs)
{
    Ent *ent;
    EC_Renderer *renderer;
    //EC_AIController *controller;
    EC_Collider *collider;
    //EC_ActorMotor *motor;
    f32 size = 1;
    Vec3 p = {};

    // Init gs and component lists
    *gs = {};
    Transform_SetToIdentity(&gs->cameraTransform);
    //Transform_SetPosition(&gs->cameraTransform, 0, 5, 18);
    Transform_SetPosition(&gs->cameraTransform, 0, -0.5f, 8);
    //Transform_SetPosition(&gs->cameraTransform, 0, 1, 0);

    // TODO: Rotation of camera in 'walk' mode is taken from
    // the input tick, each frame, so doing manual rotations of the
    // camera transform here will be useless.
    //Transform_SetRotationDegrees(&gs->cameraTransform, -45, 0, 0);
    //Transform_RotateX(&gs->cameraTransform, 45);

    gs->entList.items = g_gameEntities;
    gs->entList.count = GAME_MAX_ENTITIES;
    gs->entList.max = GAME_MAX_ENTITIES;

    gs->rendererList.items = g_renderers;
    gs->rendererList.count = GAME_MAX_ENTITIES;
    gs->rendererList.max = GAME_MAX_ENTITIES;

    gs->aiControllerList.items = g_aiControllers;
    gs->aiControllerList.count = GAME_MAX_ENTITIES;
    gs->aiControllerList.max = GAME_MAX_ENTITIES;

    gs->colliderList.items = g_colliders;
    gs->colliderList.count = GAME_MAX_ENTITIES;
    gs->colliderList.max = GAME_MAX_ENTITIES;

    gs->actorMotorList.items = g_actorMotors;
    gs->actorMotorList.count = GAME_MAX_ENTITIES;
    gs->actorMotorList.max = GAME_MAX_ENTITIES;

    gs->projectileList.items = g_prjControllers;
    gs->projectileList.count = GAME_MAX_ENTITIES;
    gs->projectileList.max = GAME_MAX_ENTITIES;
    
    gs->labelList.items = g_entLabels;
    gs->labelList.count = GAME_MAX_ENTITIES;
    gs->labelList.max = GAME_MAX_ENTITIES;
    
    const f32 testArenaWidth = 12;
    const f32 testArenaHeight = 4;
    const f32 wallHalfWidth = 0.5f;

    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    // Build Entities
    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // Walls mesh
    /////////////////////////////////////////////////////////////
    ent = Ent_GetFreeEntity(&gs->entList);
    Transform_SetPosition(&ent->transform, 0, 0, 0);
    Transform_SetScale(&ent->transform, testArenaWidth, testArenaHeight, testArenaWidth);
    
    // renderer = EC_AddRenderer(ent, gs);
    // RendObj_SetAsMesh(&renderer->rendObj, &g_meshInverseCube, 1, 1, 1, 5);

    /////////////////////////////////////////////////////////////
    // Room
    /////////////////////////////////////////////////////////////
    // Infinite plane
#if 0
    ent = Ent_GetFreeEntity(&gs->entList);
    p = {};
    p.y = -2;
    ent->transform.pos.y = p.y;
    collider = EC_AddCollider(ent, gs);
    collider->size = {testArenaWidth, 0.01f, testArenaWidth};
    collider->shapeId = Phys_CreateInfinitePlane(-2, ent->entId.index, ent->entId.iteration);
#endif

    GCmd_SpawnWorldCube cube;

    // Floor
    

#if 1
    cube = {};
    cube.pos.y = -2;
    cube.size = { 48, 1, 48 };
    Spawn_WorldCube(gs, &cube);
#endif

    // Ceiling
    cube = {};
    cube.pos.y = 12;
    cube.size = { 48, 1, 48 };
    Spawn_WorldCube(gs, &cube);

/////////////////////////////////////////////////////////////
// Player
/////////////////////////////////////////////////////////////
#if 1
    ent = Ent_GetFreeEntity(&gs->entList);
    Transform_SetPosition(&ent->transform, 0, 0, 0);
    collider = EC_AddCollider(ent, gs);
    collider->size = {1, 2, 1};
    collider->shapeId = Phys_CreateBox(
        0, 0, 0,
        0.5, 1, 0.5,
        ZCOLLIDER_FLAG_NO_ROTATION,
        COLLISION_LAYER_WORLD,
        COL_MASK_ACTOR,
        ent->entId.index, ent->entId.iteration);

    renderer = EC_AddRenderer(ent, gs);
    RendObj_SetAsBillboard(&renderer->rendObj, 1, 0, 0, 5);

    // motor = EC_AddActorMotor(ent, gs);
    // motor->speed = 5;

    gs->playerEntityIndex = ent->entId.index;
#endif
    /////////////////////////////////////////////////////////////

#if 1
    for (int i = 0; i < 20; ++i)
    {
        GCmd_Spawn cmd = {};
        cmd.entityType = ENTITY_TYPE_RIGIDBODY_CUBE;
        cmd.pos = Game_RandomSpawnOffset(10, 0, 10);
        cmd.pos.y += 10;

        Cmd_Spawn(gs, &cmd);
    }
#endif
}

void Game_BuildTestHud(GameState *state)
{
#if 0
    Ent* ent;
    EC_Renderer* renderer;
    
    // Init gs and component lists
    *state = {};

    state->entList.items = g_uiEntities;
    state->entList.count = UI_MAX_ENTITIES;
    state->entList.max = UI_MAX_ENTITIES;

    state->rendererList.items = g_ui_renderers;
    state->rendererList.count = UI_MAX_ENTITIES;
    state->rendererList.max = UI_MAX_ENTITIES;
#endif

/////////////////////////////////////////////////////////////
// A test HUD
/////////////////////////////////////////////////////////////
#if 0
    // UI
    ent = Ent_GetFreeEntity(&state->entList);
    ent->transform.pos.z = 0;
    ent->transform.scale.x = 0.1f;
    ent->transform.scale.y = 0.1f;
    ent->transform.scale.z = 0.1f;

    renderer = EC_AddRenderer(ent, state);
    // RendObj_SetAsMesh(&renderer->rendObj, &g_meshOctahedron, 1, 1, 1, 2);

    RendObj_SetAsSprite(&renderer->rendObj, SPRITE_MODE_UI, 4, 1, 1);
    //RendObj_SetSpriteUVs(&obj->obj.sprite, 0.0625, 0.125, 0.5625, 0.5625 + 0.0625);
    RendObj_CalculateSpriteAsciUVs(&renderer->rendObj.data.sprite, '.');
#endif

#if 0
    // Sentence
    ent = Ent_GetFreeEntity(&state->entList);
    ent->transform.pos.x = -1;
    ent->transform.pos.y = 1;
    renderer = EC_AddRenderer(ent, state);

    char* chars = g_debugStr->chars;
    i32 numChars = COM_StrLen(chars);
	//i32 numChars = g_testString;

    RendObj_SetAsAsciCharArray(&renderer->rendObj, chars, numChars, 0.05f);
    //obj->transform.pos.x = -1;//-0.75f;
    //obj->transform.pos.y = 1;//0.75f;
#endif

#if 0
    /////////////////////////////////////////////////////////////
    // Walls mesh
    /////////////////////////////////////////////////////////////
    ent = Ent_GetFreeEntity(&state->entList);

    ent->transform.pos.x = 0;
    ent->transform.pos.y = 0;
    ent->transform.pos.z = 0;
    ent->transform.scale.x = 12;
    ent->transform.scale.y = 4;
    ent->transform.scale.z = 12;

    renderer = EC_AddRenderer(ent, state);
    
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshInverseCube, 1, 1, 1, 5);
#endif
}

void Game_BuildTestMenu()
{
    
}

void Game_Init(Heap* globalHeap)
{
    // BlockRef ref = {};
    // Heap_Allocate(globalHeap, &ref, MegaBytes(1), "PHYS CMD");
    
    Game_BuildTestScene(&g_gameState);
    Game_BuildTestHud(&g_uiState);
    Game_BuildTestMenu();
}

void Game_Shutdown()
{
    Phys_Shutdown();
}

inline void Game_HandleEntityUpdate(GameState *gs, PhysEV_TransformUpdate *ev)
{
    EntId entId = {};
    entId.index = ev->ownerId;
    entId.iteration = ev->ownerIteration;
    Ent *ent = Ent_GetEntity(&gs->entList, &entId);
    if (ent == NULL)
    {
        return;
    }
    M4x4* updateM = (M4x4*)&ev->matrix;
	f32 magX = Vec3_Magnitudef(updateM->x0, updateM->x1, updateM->x2);
	f32 magY = Vec3_Magnitudef(updateM->y0, updateM->y1, updateM->y2);
	f32 magZ = Vec3_Magnitudef(updateM->z0, updateM->z1, updateM->z2);
	if (ZABS(magX) > 1.1f)
	{
		int foo = 1;
	}
	if (ZABS(magY) > 1.1f)
	{
		int foo = 1;
	}
	if (ZABS(magZ) > 1.1f)
	{
		int foo = 1;
	}
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
// Write Debug String
/////////////////////////////////////////////////////////////////////////////
#define GAME_DEBUG_MODE_NONE 0
#define GAME_DEBUG_MODE_CAMERA 1
#define GAME_DEBUG_MODE_TRANSFORM 2
#define GAME_DEBUG_MODE_PHYSICS 3

ZStringHeader Game_WriteDebugString(GameState *gs, GameTime *time)
{
    //Vec3 pos, rot;

    ZStringHeader h;
    h.chars = gs->debugString;
    h.length = 0;
#if 1
    switch (gs->debugMode)
    {
        #if 0
        case GAME_DEBUG_MODE_CAMERA:
        {
            pos = gs->cameraTransform.pos;
            rot = gs->cameraTransform.rot;
            AngleVectors vectors = {};
            Calc_AnglesToAxesZYX(&rot, &vectors.left, &vectors.up, &vectors.forward);
            h.length = sprintf_s(gs->debugString, gs->debugStringCapacity,
                                 "Game.DLL:\nTimeDelta: %3.7f\n-- Camera --\nPos: %3.3f, %3.3f, %3.3f\nRot: %3.3f, %3.3f, %3.3f\nForward: %3.3f, %3.3f, %3.3f\nUp: %3.3f, %3.3f, %3.3f\nLeft: %3.3f, %3.3f, %3.3f\n",
                                 time->deltaTime,
                                 pos.x, pos.y, pos.z,
                                 rot.x, rot.y, rot.z,
                                 vectors.forward.x, vectors.forward.y, vectors.forward.z,
                                 vectors.up.x, vectors.up.y, vectors.up.z,
                                 vectors.left.x, vectors.left.y, vectors.left.z);
        }
        break;
        #endif

        case GAME_DEBUG_MODE_PHYSICS:
        {
            // char* str;
            // u32 numChars;
            Phys_GetDebugString(&h.chars, &h.length);
            
        } break;

        case GAME_DEBUG_MODE_TRANSFORM:
        {
            Ent *ent = Ent_GetEntityByTag(&gs->entList, 10);
            M3x3 rotation = {};
            Vec3 inputRot = {};
            Vec3 pos = {};
            Vec3 rot = {};
            Vec3 scale = {};
            if (ent == NULL)
            {
                rotation = gs->cameraTransform.rotation;
                inputRot = g_debugInput.degrees;
                scale = gs->cameraTransform.scale;
                pos = gs->cameraTransform.pos;
                rot = Transform_GetEulerAnglesDegrees(&gs->cameraTransform);
            }
            else
            {
                rotation = ent->transform.rotation;
                inputRot = g_debugInput.degrees;
                scale = ent->transform.scale;
                pos = ent->transform.pos;
                rot = Transform_GetEulerAnglesDegrees(&ent->transform);
                if (isnan(rot.x))
                {
                    //App_ErrorStop();
                }
            }
                
                
                // mRot.z = atan2f(m[1], m[5]);
                // mRot.y = atan2f(m[8], m[10]);
                // mRot.x = -asinf(m[9]);
                
                //AngleVectors vectors = {};
                h.length = sprintf_s(gs->debugString, gs->debugStringCapacity,
"Game.DLL:\nTimeDelta: %3.7f\n\
Input Rot: %3.3f, %3.3f, %3.3f\n\
-- Debug Transform --\n\
pos: %3.3f, %3.3f, %3.3f\n\
euler: %3.3f, %3.3f, %3.3f\n\
scale: %3.3f, %3.3f, %3.3f\n\
Rotation:\n\
(X0) %3.3f, (Y0) %3.3f, (Z0) %3.3f\n\
(X1) %3.3f, (Y1) %3.3f, (Z1) %3.3f\n\
(X2) %3.3f, (Y2) %3.3f, (Z2) %3.3f\n\
",
                                     time->deltaTime,
                                     inputRot.x, inputRot.y, inputRot.z,
                                     pos.x, pos.y, pos.z,
                                     rot.x, rot.y, rot.z,
                                     //scale.x, scale.y, scale.z,
                                     scale.x, scale.y, scale.z,
                                     rotation.xAxisX, rotation.yAxisX, rotation.zAxisX,
                                     rotation.xAxisY, rotation.yAxisY, rotation.zAxisY,
                                     rotation.xAxisZ, rotation.yAxisZ, rotation.zAxisZ
                                     );
            //

        }
        break;

        default:
        {
            h.length = 0;
            return h;
        }
        break;
    }
#endif
    return h;
}

void Game_UpdateClientTick(InputActionSet* actions, ClientTick* tick)
{
    
}

u8 Game_ReadCmd(GameState* gs, u32 type, u8* ptr, u32 bytes)
{
    switch (type)
    {
        case CMD_TYPE_SPAWN:
        {
            GCmd_Spawn cmd = {};
            Assert(sizeof(GCmd_Spawn) == bytes);
            COM_COPY_STRUCT(ptr, &cmd, GCmd_Spawn);
            Cmd_Spawn(gs, &cmd);
            return 1;
        } break;

        case CMD_TYPE_SPAWN_WORLD_CUBE:
        {
            GCmd_SpawnWorldCube cmd = {};
            Assert(sizeof(GCmd_SpawnWorldCube) == bytes);
            COM_COPY_STRUCT(ptr, &cmd, GCmd_SpawnWorldCube);
            Spawn_WorldCube(gs, &cmd);
        } break;
    }
    return 0;
}

#define MAX_ALLOWED_PHYSICS_STEP 0.0334f
/////////////////////////////////////////////////////////////////////////////
// Game Tick
/////////////////////////////////////////////////////////////////////////////
void Game_Tick(
    GameState *gs,
    ByteBuffer input,
    ByteBuffer* output,
    GameTime *time,
    InputActionSet* actions)
{
    Game_ApplyInputToTransform(actions, &g_clientTick, &gs->cameraTransform, time);

    //if (input->attack2)
    if (Input_CheckActionToggledOn(actions, "Attack 2", time->frameNumber))
    {
        i32 shapeId = 10;
        Transform t = gs->cameraTransform;
        Phys_TeleportShape(10, t.pos.x, t.pos.y, t.pos.z);

		Vec3 forward = gs->cameraTransform.rotation.zAxis;
		f32 speed = 20;
		Phys_ChangeVelocity(shapeId, speed * (-forward.x), speed * (-forward.y), speed * (-forward.z));
    }
    
    g_debugTransform = gs->cameraTransform;
    
    // Force physics step to always be no lower than 30fps
    f32 dt = time->deltaTime;
    if (dt > MAX_ALLOWED_PHYSICS_STEP)
    {
        dt = MAX_ALLOWED_PHYSICS_STEP;
    }

    MemoryBlock eventBuffer = Phys_Step(dt);
    
    i32 ptrOffset = 0;
    u8 reading = 1;
	i32 eventsProcessed = 0;
    while (reading && ptrOffset < eventBuffer.size)
    {
        u8 *mem = (u8 *)((u8 *)eventBuffer.ptrMemory + ptrOffset);
        i32 type = *(i32 *)mem;
        switch (type)
        {
        case 1:
        {
            PhysEV_TransformUpdate tUpdate = {};
            COM_CopyMemory(mem, (u8 *)&tUpdate, sizeof(PhysEV_TransformUpdate));
            ptrOffset += sizeof(PhysEV_TransformUpdate);
            Game_HandleEntityUpdate(gs, &tUpdate);
			eventsProcessed++;
        }
        break;

        default:
        {
            reading = 0;
        }
        break;
        }
    }

    // Game state update
    // Update all inputs, entity components and colliders/physics
    Game_UpdateActorMotors(gs, time, &g_clientTick);
    Ent_UpdateAIControllers(gs, time);
    Game_UpdateColliders(gs, time);
    Game_UpdateProjectiles(gs, time);
    //Game_UpdateAI(time);

    if (Input_CheckActionToggledOn(actions, "Spawn Test", time->frameNumber))
    {
        BufferItemHeader header = {};
        header.type = CMD_TYPE_SPAWN;
        header.size = sizeof(GCmd_Spawn);
        GCmd_Spawn cmd = {};
        cmd.entityType = ENTITY_TYPE_RIGIDBODY_CUBE;
        cmd.pos = Game_RandomSpawnOffset(10, 0, 10);
        cmd.pos.y += 10;

        output->ptrWrite += COM_COPY_STRUCT(&header, output->ptrWrite, BufferItemHeader);
        output->ptrWrite += COM_COPY_STRUCT(&cmd, output->ptrWrite, GCmd_Spawn);
        output->count++;
    }
}
