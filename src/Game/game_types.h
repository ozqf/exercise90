#pragma once

#include "../Shared/shared.h"


struct Ent
{
    i32 id;
    Transform transform;
    u32 components;
};

struct EntComp_TestBrain
{
    i32 id;
    
};

struct World
{
    i32 nextEntityID;
    Ent* entities;
    i32 maxEntities;
    i32 lastEntityIndex;

};

void World_Init(World* world, Ent* entityList, i32 maxEntities)
{
    *world = {};
    world->entities = entityList;
    world->maxEntities = maxEntities;
}
