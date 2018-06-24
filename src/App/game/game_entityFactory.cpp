#pragma once

#include "game.h"

#if 1
// typedeffing a function of signature: i32* Foo(i32* param1);
//typedef i32* (Foo)(i32* param1);
typedef Ent* (Ent_ReadStaticState)(GameState* gs, Cmd_Spawn* cmd);
typedef Ent* (Ent_ReadDynamicState)(GameState* gs, Cmd_EntityState* cmd);

Ent_ReadDynamicState* g_spawnFunctions[12];
i32 g_numSpawnFunctions = 0;
#endif

Ent* Spawn_WorldCube(GameState* gs, Cmd_Spawn* cmd)
{
    //Ent* ent = Ent_GetFreeEntity(&gs->entList);
    Ent* ent = Ent_GetAndAssign(&gs->entList, &cmd->entityId);
    ent->transform.pos = cmd->pos;
    
    EC_Renderer* renderer = EC_AddRenderer(gs, ent);
    //RendObj_SetAsMesh(&renderer->rendObj, &g_meshCube, 1, 1, 1, 7);
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshCube, 1, 1, 1, AppGetTextureIndexByName("textures\\COMP03_1.bmp"));
    Transform_SetScale(&ent->transform, cmd->size.x, cmd->size.y, cmd->size.z);

    EC_Collider* collider = EC_AddCollider(gs, ent);
	collider->size = { cmd->size.x, cmd->size.y, cmd->size.z };
    collider->shapeId = Phys_CreateBox(
        cmd->pos.x, cmd->pos.y, cmd->pos.z,
        cmd->size.x / 2.0f, cmd->size.y / 2.0f, cmd->size.z / 2.0f,
        ZCOLLIDER_FLAG_STATIC,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEFAULT,
        ent->entId.index, ent->entId.iteration
    );
    return ent;
}

#if 0
Ent* Spawn_RigidbodyCube(GameState* gs, Cmd_Spawn* cmd)
{
    //Ent* ent = Ent_GetFreeEntity(&gs->entList);
    Ent* ent = Ent_GetAndAssign(&gs->entList, &cmd->entityId);

    Transform_SetPosition(&ent->transform, cmd->pos.x, cmd->pos.y, cmd->pos.z);
    
    Transform_SetScale(&ent->transform, 1, 1, 1);
    Transform_SetRotationDegrees(&ent->transform, 0, 0, 0);
    
    //controller = EC_AddAIController(ent, gs);
    //Ent_InitAIController(controller, 1, 0, 0, 5);

    EC_Renderer* renderer = EC_AddRenderer(gs, ent);
	//RendObj_SetAsMesh(&renderer->rendObj, &g_meshCube, 1, 1, 1, 6);
	RendObj_SetAsMesh(&renderer->rendObj, &g_meshCube, 1, 1, 1, AppGetTextureIndexByName("textures\\W33_5.bmp"));

    f32 size = 1.0f;
    EC_Collider* collider = EC_AddCollider(gs, ent);
    collider->size = { size, size, size };
    collider->shapeId = Phys_CreateBox(
        ent->transform.pos.x,
        ent->transform.pos.y,
        ent->transform.pos.z,
        size / 2, size / 2, size / 2,
        0,
        COLLISION_LAYER_WORLD,
        COL_MASK_DEBRIS,
        ent->entId.index,
        ent->entId.iteration);
    return ent;
}
#endif
#if 0
Ent* Spawn_GroundActor(GameState* gs, Cmd_Spawn* cmd)
{
    //Ent *ent;
    //EC_Renderer *renderer;
    //EC_AIController *controller;
    EC_Collider *collider;
    EC_ActorMotor* motor;

    //ent = Ent_GetFreeEntity(&gs->entList);
    Ent* ent = Ent_GetAndAssign(&gs->entList, &cmd->entityId);
    Transform_SetPosition(&ent->transform, 0, 0, 0);
    collider = EC_AddCollider(gs, ent);
	f32 playerHeight = 1.85f; // average male height in metres
	f32 playerWidth = 0.46f; // reasonable shoulder width?
    collider->size = { playerWidth, playerHeight, playerWidth };
    collider->shapeId = Phys_CreateBox(
        0, 0, 0,
        playerWidth / 2, playerHeight / 2, playerWidth / 2,
        ZCOLLIDER_FLAG_NO_ROTATION,
        COLLISION_LAYER_WORLD,
        COL_MASK_ACTOR,
        ent->entId.index, ent->entId.iteration);

    //renderer = EC_AddRenderer(gs, ent);
    //RendObj_SetAsBillboard(&renderer->rendObj, 1, 1, 1, AppGetTextureIndexByName("textures\\brbrick2.bmp"));

    motor = EC_AddActorMotor(gs, ent);
    motor->runSpeed = 10;
    motor->runAcceleration = 50;

    gs->localPlayerHasEnt = 1;
    gs->localPlayerEntId = ent->entId;
    return ent;
}
#endif
Ent* Spawn_Null(GameState* gs, Cmd_Spawn* cmd)
{
    ILLEGAL_CODE_PATH
    return NULL;
}

void Game_InitEntityFactory()
{
    // Make sure these align with the defines for these types
    // defined in game_commands.h
    // g_spawnFunctions[ENTITY_TYPE_NULL] = Spawn_Null;
    // g_spawnFunctions[ENTITY_TYPE_WORLD_CUBE] = Spawn_WorldCube;
    // g_spawnFunctions[ENTITY_TYPE_RIGIDBODY_CUBE] = Spawn_RigidbodyCube;
    // g_spawnFunctions[ENTITY_TYPE_ACTOR_GROUND] = Spawn_GroundActor;
    // g_numSpawnFunctions = ENTITY_TYPES_COUNT;
}

Ent* Exec_StaticEntityState(GameState* gs, Cmd_Spawn* cmd)
{
    if (g_verbose)
    {
        printf("GAME STATIC STATE id %d/%d type %d\n",
            cmd->entityId.iteration,
            cmd->entityId.index,
            cmd->factoryType
        );
    }
    if (cmd->factoryType != ENTITY_TYPE_WORLD_CUBE)
    {
        printf("GAME factory type %d is not a valid static entity. Aborting\n", cmd->factoryType);
        ILLEGAL_CODE_PATH
    }
    // Only one type atm
	Ent* ent = Spawn_WorldCube(gs, cmd);
	ent->factoryType = cmd->factoryType;
	return ent;
#if 0
    if (g_verbose)
    {
        printf("GAME SPAWN id %d/%d type %d\n",
            cmd->entityId.iteration,
            cmd->entityId.index,
            cmd->factoryType
        );
    }
    if (cmd->factoryType >= g_numSpawnFunctions || cmd->factoryType < ENTITY_TYPE_NULL)
    {
        ILLEGAL_CODE_PATH
    }
    Ent* ent = g_spawnFunctions[cmd->factoryType](gs, cmd);
    if (ent != NULL)
    {
        ent->factoryType = cmd->factoryType;
    }
    else
    {
        ILLEGAL_CODE_PATH
    }
    return ent;
#endif
}

Ent* Ent_ReadGroundActorState(GameState* gs, Cmd_EntityState* cmd)
{
    Ent* ent = Ent_GetEntityById(&gs->entList, &cmd->entityId);

    if (ent != NULL)
    {
        // restore state
        //Transform_SetPosition(&ent->transform, cmd->pos.x, cmd->pos.y, cmd->pos.z);
        //Transform_SetRotationDegrees(&ent->transform, 0, 0, 0);

        EC_Collider *collider = EC_FindCollider(gs, ent);
        PhysCmd_State s = {};
        s.shapeId = collider->shapeId;
        
        Phys_SetState(&s);
    }
    else
    {
        EC_Collider *collider;
        EC_ActorMotor* motor;

        //ent = Ent_GetFreeEntity(&gs->entList);
        ent = Ent_GetAndAssign(&gs->entList, &cmd->entityId);
        Transform_SetPosition(&ent->transform, 0, 0, 0);
        collider = EC_AddCollider(gs, ent);
	    f32 playerHeight = 1.85f; // average male height in metres
	    f32 playerWidth = 0.46f; // reasonable shoulder width?
        collider->size = { playerWidth, playerHeight, playerWidth };
        collider->shapeId = Phys_CreateBox(
            0, 0, 0,
            playerWidth / 2, playerHeight / 2, playerWidth / 2,
            ZCOLLIDER_FLAG_NO_ROTATION,
            COLLISION_LAYER_WORLD,
            COL_MASK_ACTOR,
            ent->entId.index, ent->entId.iteration);

        //renderer = EC_AddRenderer(gs, ent);
        //RendObj_SetAsBillboard(&renderer->rendObj, 1, 1, 1, AppGetTextureIndexByName("textures\\brbrick2.bmp"));

        motor = EC_AddActorMotor(gs, ent);
        motor->runSpeed = 10;
        motor->runAcceleration = 50;

        gs->localPlayerHasEnt = 1;
        gs->localPlayerEntId = ent->entId;
    }
    return ent;
}

Ent* Ent_ReadRigidBodyCubeState(GameState* gs, Cmd_EntityState* cmd)
{
    Ent* ent = Ent_GetEntityById(&gs->entList, &cmd->entityId);

    if (ent != NULL)
    {
        // restore state
        Transform_SetPosition(&ent->transform, cmd->pos.x, cmd->pos.y, cmd->pos.z);
        Transform_SetRotationDegrees(&ent->transform, 0, 0, 0);
        
    }
    else
    {
        // Spawn
        ent = Ent_GetAndAssign(&gs->entList, &cmd->entityId);

        Transform_SetPosition(&ent->transform, cmd->pos.x, cmd->pos.y, cmd->pos.z);

        Transform_SetScale(&ent->transform, 1, 1, 1);
        Transform_SetRotationDegrees(&ent->transform, 0, 0, 0);
        
        //controller = EC_AddAIController(ent, gs);
        //Ent_InitAIController(controller, 1, 0, 0, 5);

        EC_Renderer* renderer = EC_AddRenderer(gs, ent);
	    //RendObj_SetAsMesh(&renderer->rendObj, &g_meshCube, 1, 1, 1, 6);
	    RendObj_SetAsMesh(&renderer->rendObj, &g_meshCube, 1, 1, 1, AppGetTextureIndexByName("textures\\W33_5.bmp"));

        f32 size = 1.0f;
        EC_Collider* collider = EC_AddCollider(gs, ent);
        collider->size = { size, size, size };
        collider->shapeId = Phys_CreateBox(
            ent->transform.pos.x,
            ent->transform.pos.y,
            ent->transform.pos.z,
            size / 2, size / 2, size / 2,
            0,
            COLLISION_LAYER_WORLD,
            COL_MASK_DEBRIS,
            ent->entId.index,
            ent->entId.iteration);
    }
    //Ent* ent = Ent_GetFreeEntity(&gs->entList);
    
    return ent;
}

Ent* Ent_ReadProjectileState(GameState* gs, Cmd_EntityState* cmd)
{
    Ent* ent = Ent_GetEntityById(&gs->entList, &cmd->entityId);

    if (ent != NULL)
    {
        // restore state
        Transform_SetPosition(&ent->transform, cmd->pos.x, cmd->pos.y, cmd->pos.z);
        Transform_SetRotationDegrees(&ent->transform, 0, 0, 0);
        
    }
    else
    {
        ent = Ent_GetAndAssign(&gs->entList, &cmd->entityId);
        ent->transform.pos.x = cmd->pos.x;
        ent->transform.pos.y = cmd->pos.y;
        ent->transform.pos.z = cmd->pos.z;

        Transform_SetScale(&ent->transform, 0.3f, 0.3f, 0.3f);
        Transform_RotateY(&ent->transform, cmd->rot.y * DEG2RAD);
        Transform_RotateX(&ent->transform, cmd->rot.x * DEG2RAD);


        EC_Renderer* rend = EC_AddRenderer(gs, ent);
        RendObj_SetAsBillboard(&rend->rendObj, 1, 1, 1, AppGetTextureIndexByName("textures\\BAL1A0.bmp"));
        rend->rendObj.flags = 0 | RENDOBJ_FLAG_DEBUG;


        EC_Projectile* prj = EC_AddProjectile(gs, ent);
        prj->tock = cmd->tock;
        prj->tick = cmd->tick;
        prj->move.x = cmd->vel.x;
        prj->move.y = cmd->vel.y;
        prj->move.z = cmd->vel.z;
    
    }
    return ent;
}

void Game_WriteEntityState(GameState* gs, Ent* ent, Cmd_EntityState* s)
{
    s->entityId = ent->entId;
    s->factoryType = ent->factoryType;
    s->tag = ent->tag;

    //
    s->pos.x = ent->transform.pos.x;
    s->pos.y = ent->transform.pos.y;
    s->pos.z = ent->transform.pos.z;

    s->size.x = ent->transform.scale.x;
    s->size.y = ent->transform.scale.y;
    s->size.z = ent->transform.scale.z;

    // Additional, factory specific
    switch (ent->factoryType)
    {
        case ENTITY_TYPE_PROJECTILE:
        {
            EC_Projectile* prj = EC_FindProjectile(gs, ent);
            Assert(prj);
            s->tick = prj->tick;
            s->tock = prj->tock;
        } break;

        case ENTITY_TYPE_ACTOR_GROUND:
        {
            EC_ActorMotor* motor = EC_FindActorMotor(gs, ent);
            Assert(motor);
        } break;
    }
}

Ent* Exec_DynamicEntityState(GameState* gs, Cmd_EntityState* cmd)
    {
    if (cmd->factoryType == ENTITY_TYPE_WORLD_CUBE)
    {
        printf("Factory type %d cannot be dynamic!\n", cmd->factoryType);
        ILLEGAL_CODE_PATH
    }
    if (gs->verbose)
    {
        printf("GAME Dynamic state for type %d Id %d/%d\n",
            cmd->factoryType,
            cmd->entityId.iteration,
            cmd->entityId.index
        );
    }
    Ent* ent = NULL;
    switch (cmd->factoryType)
    {
        case ENTITY_TYPE_PROJECTILE:
        {
            ent = Ent_ReadProjectileState(gs, cmd);
        } break;

        case ENTITY_TYPE_ACTOR_GROUND:
        {
            ent = Ent_ReadGroundActorState(gs, cmd);
        } break;

        case ENTITY_TYPE_RIGIDBODY_CUBE:
        {
            ent = Ent_ReadRigidBodyCubeState(gs, cmd);
        } break;

        default:
        {
            printf("GAME Unknown factory type: %d\n", cmd->factoryType);
            ILLEGAL_CODE_PATH
        }
    }
    if (ent != NULL)
    {
        ent->factoryType = cmd->factoryType;
    }
    return ent;
}
