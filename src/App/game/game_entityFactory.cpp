#pragma once

#include "game.h"

#if 1
// typedeffing a function of signature: i32* Foo(i32* param1);
//typedef i32* (Foo)(i32* param1);
typedef Ent* (Game_SpawnFunction)(GameState* gs, GCmd_Spawn* cmd);

Game_SpawnFunction* g_spawnFunctions[12];
i32 g_numSpawnFunctions = 0;
#endif


Ent* Spawn_WorldCube(GameState* gs, GCmd_Spawn* cmd)
{
    Ent* ent = Ent_GetFreeEntity(&gs->entList);
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

Ent* Spawn_RigidbodyCube(GameState* gs, GCmd_Spawn* cmd)
{
    Ent* ent = Ent_GetFreeEntity(&gs->entList);

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

Ent* Spawn_Null(GameState* gs, GCmd_Spawn* cmd)
{
    ILLEGAL_CODE_PATH
    return NULL;
}

void Game_InitEntityFactory()
{
    g_spawnFunctions[g_numSpawnFunctions] = Spawn_Null;
    g_numSpawnFunctions++;
    g_spawnFunctions[g_numSpawnFunctions] = Spawn_WorldCube;
    g_numSpawnFunctions++;
    g_spawnFunctions[g_numSpawnFunctions] = Spawn_RigidbodyCube;
    g_numSpawnFunctions++;
}

Ent* Cmd_Spawn(GameState* gs, GCmd_Spawn* cmd)
{
    if (cmd->factoryType >= g_numSpawnFunctions || cmd->factoryType < 0)
    {
        ILLEGAL_CODE_PATH
    }
    return g_spawnFunctions[cmd->factoryType](gs, cmd);
#if 0
    switch (cmd->factoryType)
    {
        case ENTITY_TYPE_NULL:
        {
            ILLEGAL_CODE_PATH
        } break;

        case ENTITY_TYPE_WORLD_CUBE:
        {
            return Spawn_WorldCube(gs, cmd);
        } break;

        case ENTITY_TYPE_RIGIDBODY_CUBE:
        {
            return Spawn_RigidbodyCube(gs, cmd);
        } break;
    
        case ENTITY_TYPE_ACTOR_GROUND:
        {

        } break;

        default:
        {
            ILLEGAL_CODE_PATH
        } break;
    }
    return NULL;
#endif
}
