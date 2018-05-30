#pragma once

#include "../../common/com_module.h"

#define COLLISION_LAYER_WORLD (1 << 0)
#define COLLISION_LAYER_ACTOR (1 << 1)
#define COLLISION_LAYER_DEBRIS (1 << 2)

internal u16 COL_MASK_DEFAULT = COLLISION_LAYER_WORLD;
internal u16 COL_MASK_ACTOR = COLLISION_LAYER_WORLD | COLLISION_LAYER_ACTOR;
internal u16 COL_MASK_DEBRIS = COLLISION_LAYER_WORLD | COLLISION_LAYER_DEBRIS;

//////////////////////////////////////////////////
// Game Commands
//////////////////////////////////////////////////

#define CMD_TYPE_SPAWN 100
#define CMD_TYPE_SPAWN_WORLD_CUBE 101

#define ENTITY_TYPE_WORLD_NULL 0
#define ENTITY_TYPE_WORLD_CUBE 1
#define ENTITY_TYPE_RIGIDBODY_CUBE 2

struct GCmd_SpawnWorldCube
{
    Vec3 pos;
    Vec3 rot;
    Vec3 size;
    u32 flags;
};

struct GCmd_Spawn
{
    i32 entityType;
    Vec3 pos;
    Vec3 rot;
    u32 flags;
};

void Cmd_Spawn(GameState* gs, GCmd_Spawn* cmd);
void Spawn_WorldCube(GameState* gs, GCmd_SpawnWorldCube* cmd);
