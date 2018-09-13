#pragma once

#include "game.h"

#if 1
// typedeffing a function of signature: i32* Foo(i32* param1);
//typedef i32* (Foo)(i32* param1);
//typedef Ent* (Ent_ReadStaticState)(GameState* gs, Cmd_Spawn* cmd);
//typedef Ent* (Ent_ReadDynamicState)(GameState* gs, Cmd_EntityState* cmd);

//Ent_ReadDynamicState* g_spawnFunctions[12];
i32 g_numSpawnFunctions = 0;
#endif
#if 0
Ent* Spawn_WorldCube(GameState* gs, Cmd_Spawn* cmd)
{
    Ent* ent = Ent_GetEntityById(&gs->entList, &cmd->entityId);
    if (ent != NULL)
    {
        // Restore state
        Assert(ent->factoryType == cmd->factoryType);

        // do nothing, it is static
        printf("Restoring static ent %d/%d\n", ent->entId.iteration, ent->entId.index);
    }
    else
    {
        // Create
        ent = Ent_GetAndAssign(&gs->entList, &cmd->entityId);
        ent->factoryType = cmd->factoryType;
        printf("Creating static ent %d/%d\n", ent->entId.iteration, ent->entId.index);
        
        ent->transform.pos = cmd->pos;

        EC_SingleRendObj* renderer = EC_AddSingleRendObj(gs, ent);
        //RendObj_SetAsMesh(&renderer->rendObj, &g_meshCube, 1, 1, 1, 7);
        RendObj_SetAsMesh(
            &renderer->rendObj,
            g_meshCube, 1, 1, 1,
            AppGetTextureIndexByName("textures\\COMP03_1.bmp")
        );
        Transform_SetScale(&ent->transform, cmd->size.x, cmd->size.y, cmd->size.z);

        EC_Collider* collider = EC_AddCollider(gs, ent);
	    collider->state.size = { cmd->size.x, cmd->size.y, cmd->size.z };
        collider->shapeId = Phys_CreateBox(
            cmd->pos.x, cmd->pos.y, cmd->pos.z,
            cmd->size.x / 2.0f, cmd->size.y / 2.0f, cmd->size.z / 2.0f,
            ZCOLLIDER_FLAG_STATIC,
            COLLISION_LAYER_WORLD,
            COL_MASK_DEFAULT,
            ent->entId.index, ent->entId.iteration
        );
    }
    //Ent* ent = Ent_GetFreeEntity(&gs->entList);
    
    return ent;
}
#endif
#if 0
Ent* Spawn_Null(GameState* gs, Cmd_Spawn* cmd)
{
    ILLEGAL_CODE_PATH
    return NULL;
}
#endif

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
#if 0
Ent* Ent_ReadThinkerState(GameState* gs, Cmd_EntityState* cmd)
{
    Ent* ent = Ent_GetEntityById(&gs->entList, &cmd->entityId);

    if (ent != NULL)
    {
        Assert(ent->factoryType == cmd->factoryType);
    }
    else
    {

    }
    return ent;
}

Ent* Exec_DynamicEntityState(GameState* gs, Cmd_EntityState* cmd)
{
    if (cmd->factoryType == ENTITY_TYPE_WORLD_CUBE)
    {
        printf("Factory type %d cannot be dynamic!\n", cmd->factoryType);
        ILLEGAL_CODE_PATH
    }
    Ent* ent = NULL;
    switch (cmd->factoryType)
    {
        case ENTITY_TYPE_THINKER:
        {
            ent = Ent_ReadThinkerState(gs, cmd);
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
#endif
