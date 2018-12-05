#pragma once

#include "game.h"

// typedeffing a function of signature: i32* Foo(i32* param1);
//typedef i32* (Foo)(i32* param1);
//typedef Ent* (Ent_ReadStaticState)(GameScene* gs, Cmd_Spawn* cmd);
//typedef Ent* (Ent_ReadDynamicState)(GameScene* gs, Cmd_EntityState* cmd);

//Ent_ReadDynamicState* g_spawnFunctions[12];

void Game_InitEntityFactory()
{
    // Make sure these align with the defines for these types
    // defined in game_commands.h
    // g_spawnFunctions[ENTITY_TYPE0_NULL] = Spawn_Null;
    // g_spawnFunctions[ENTITY_TYPE1_WORLD_CUBE] = Spawn_WorldCube;
    // g_spawnFunctions[ENTITY_TYPE2_RIGIDBODY_CUBE] = Spawn_RigidbodyCube;
    // g_spawnFunctions[ENTITY_TYPE3_ACTOR_GROUND] = Spawn_GroundActor;
    // g_numSpawnFunctions = ENTITY_TYPES_COUNT;
}
