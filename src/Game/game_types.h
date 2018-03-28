#pragma once

#include "../Shared/shared.h"

// Max of 32 components
#define ECOMP_FLAG_RENDOBJ          (1 << 0)
#define ECOMP_FLAG_AI_CONTROLLER    (1 << 1)
#define ECOMP_FLAG_COLLIDER         (1 << 2)

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
union EntId
{
    struct
    {
        u16 iteration;
        u16 index;
    };
    u16 arr[2];
};

inline u8 EntIdsEqual(EntId a, EntId b)
{
    return (a.iteration == b.iteration && a.index == b.index);
}

struct Ent
{
    EntId entId;
    u8 inUse;               // if 0 this entity is free to be recycled
    Transform transform;
    u32 componentFlags;
};

// A quick test component
struct EntComp_AIController
{
    EntId entId;
    u8 inUse;
    Vec3 dir;
    f32 speed;
};

struct EntComp_Collider
{
    i32 someValue;
};

struct Ent_Renderer
{
    RendObj rendObj;
};

struct GameState
{
    i32 nextEntityID;
    Ent* entities;
    i32 maxEntities;
    i32 lastEntityIndex;

};

void World_Init(GameState* gs, Ent* entityList, i32 maxEntities)
{
    *gs = {};
    gs->entities = entityList;
    gs->maxEntities = maxEntities;
}
