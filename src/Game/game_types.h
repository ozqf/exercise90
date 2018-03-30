#pragma once

#include "../Shared/shared.h"

/*
Entity system:
Entities are a combination of components grouped via an ent id
To create an entity:
> Allocate/find a free ent slot and assign it, getting the id.
> Allocate/find free components and assign it the entity id.
*/

//////////////////////////////////////////////////
// Define Entity
// index: direct index into the entity array
// iteration: Incremented for each use of this entity index
//   to make recycled uses unique
//////////////////////////////////////////////////
union EntId
{
    struct
    {
        u16 iteration;
        u16 index;
    };
    u16 arr[2];
};

inline void EntId_Copy(EntId* source, EntId* target)
{
    target->index = source->index;
    target->iteration = source->iteration;
}

inline u8 EntId_Equals(EntId* a, EntId* b)
{
    return (a->index == b->index && a->iteration == b->iteration);
}

struct Ent
{
    EntId entId;
    u8 inUse;               // if 0 this entity is free to be recycled
    Transform transform;
    u32 componentFlags;
};

//////////////////////////////////////////////////
// Define Components
// All components required:
//   EntId entId;
//   u8 inUse;
//////////////////////////////////////////////////

// Max of 32 components per entity

struct World;
#define COMP_FLAG_AICONTROLLER (1 << 0)
#define COMP_FLAG_COLLIDER (1 << 1)
#define COMP_FLAG_RENDERER (1 << 2)

// A quick test component
struct EC_AIController
{
    EntId entId;
    u8 inUse;
    Vec3 dir;
    f32 speed;
};

struct EC_Collider
{
    EntId entId;
    u8 inUse;
    Vec3 size;
    i32 isOverlapping;
};

struct EC_Renderer
{
    EntId entId;
    u8 inUse;
    RendObj rendObj;
};

//////////////////////////////////////////////////
// Define component list structs and World objects...
//////////////////////////////////////////////////
// World will require component lists to be defined already!
#include "game_entComponentBase.h"
DEFINE_ENT_COMPONENT_LIST(AIController)
DEFINE_ENT_COMPONENT_LIST(Renderer)
DEFINE_ENT_COMPONENT_LIST(Collider)

struct World
{
    EC_AIControllerList aiControllerList;
    EC_RendererList rendererList;
    EC_ColliderList colliderList;
};

//////////////////////////////////////////////////
// ...and Component Add/Remove/Has functions.
//////////////////////////////////////////////////
DEFINE_ENT_COMPONENT_BASE(AIController, aiController, COMP_FLAG_AICONTROLLER)
DEFINE_ENT_COMPONENT_BASE(Collider, collider, COMP_FLAG_COLLIDER)
DEFINE_ENT_COMPONENT_BASE(Renderer, renderer, COMP_FLAG_RENDERER)


// Defined last as it contains macro defined structs from above
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
