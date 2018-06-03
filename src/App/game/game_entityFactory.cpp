#pragma once

#include "game.h"

#if 1
// typedeffing a function of signature: i32* Foo(i32* param1);
//typedef i32* (Foo)(i32* param1);
typedef Ent* (Game_SpawnFunction)(GameState* gs, Cmd_Spawn* cmd);

Game_SpawnFunction* g_spawnFunctions[12];
i32 g_numSpawnFunctions = 0;
#endif

Ent* Spawn_WorldCube(GameState* gs, Cmd_Spawn* cmd)
{
    //Ent* ent = Ent_GetFreeEntity(&gs->entList);
    Ent* ent = Ent_GetEntityAndAssign(&gs->entList, &cmd->entityId);
    ent->transform.pos = cmd->pos;
    
    EC_Renderer* renderer = EC_AddRenderer(ent, gs);
    RendObj_SetAsMesh(&renderer->rendObj, &g_meshCube, 1, 1, 1, 5);
    Transform_SetScale(&ent->transform, cmd->size.x, cmd->size.y, cmd->size.z);

    EC_Collider* collider = EC_AddCollider(ent, gs);
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

Ent* Spawn_RigidbodyCube(GameState* gs, Cmd_Spawn* cmd)
{
    //Ent* ent = Ent_GetFreeEntity(&gs->entList);
    Ent* ent = Ent_GetEntityAndAssign(&gs->entList, &cmd->entityId);

    Transform_SetPosition(&ent->transform, cmd->pos.x, cmd->pos.y, cmd->pos.z);
    
    Transform_SetScale(&ent->transform, 1, 1, 1);
    Transform_SetRotationDegrees(&ent->transform, 0, 0, 0);
    
    //controller = EC_AddAIController(ent, gs);
    //Ent_InitAIController(controller, 1, 0, 0, 5);

    EC_Renderer* renderer = EC_AddRenderer(ent, gs);
	RendObj_SetAsMesh(&renderer->rendObj, &g_meshCube, 1, 1, 1, 5);

    f32 size = 1.0f;
    EC_Collider* collider = EC_AddCollider(ent, gs);
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

Ent* Spawn_GroundActor(GameState* gs, Cmd_Spawn* cmd)
{
    //Ent *ent;
    EC_Renderer *renderer;
    //EC_AIController *controller;
    EC_Collider *collider;

    //ent = Ent_GetFreeEntity(&gs->entList);
    Ent* ent = Ent_GetEntityAndAssign(&gs->entList, &cmd->entityId);
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
    return ent;
}

Ent* Spawn_Null(GameState* gs, Cmd_Spawn* cmd)
{
    ILLEGAL_CODE_PATH
    return NULL;
}

void Game_InitEntityFactory()
{
    // Make sure these align with the defines for these types
    // defined in game_commands.h
    g_spawnFunctions[ENTITY_TYPE_NULL] = Spawn_Null;
    g_spawnFunctions[ENTITY_TYPE_WORLD_CUBE] = Spawn_WorldCube;
    g_spawnFunctions[ENTITY_TYPE_RIGIDBODY_CUBE] = Spawn_RigidbodyCube;
    g_spawnFunctions[ENTITY_TYPE_ACTOR_GROUND] = Spawn_GroundActor;
    g_numSpawnFunctions = ENTITY_TYPES_COUNT;
}

Ent* Exec_Spawn(GameState* gs, Cmd_Spawn* cmd)
{
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
}
