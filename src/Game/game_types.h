#pragma once

#include "../Shared/shared.h"

/*
Entity system:
Entities are a combination of components grouped via an ent id
To create an entity:
> Allocate/find a free ent slot and assign it, getting the id.
> Allocate/find free components and assign it the entity id.
*/

/**
 * iteration: Incremented for each use of this entity index
 * index: direct index into the entity array
 */
struct EntId
{
    u16 iteration;
    u16 index;
};

struct Ent
{
    EntId entId;
    u8 inUse;               // if 0 this entity is free to be recycled
    Transform transform;
    u32 components;
};

// A quick, test component
struct EntComp_TestBrain
{
    EntId entId;
    
};

struct EntComp_Collider
{
    i32 someValue;
};

struct Ent_Renderer
{
    RendObj rendObj;
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
