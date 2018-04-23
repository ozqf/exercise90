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
    i32 tag;
};

struct EntList
{
    Ent* items;
    u32 count;
    u32 max;
};

//////////////////////////////////////////////////
// Define Components
// All components required:
//   EntId entId;
//   u8 inUse;
//////////////////////////////////////////////////

// Max of 32 components per entity

//struct GameState;
#define COMP_FLAG_AICONTROLLER (1 << 0)
#define COMP_FLAG_COLLIDER (1 << 1)
#define COMP_FLAG_RENDERER (1 << 2)
#define COMP_FLAG_ACTORMOTOR (1 << 3)
#define COMP_FLAG_PROJECTILE (1 << 4)

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
    i32 shapeId;

    u32 lastFrameOverlapping;
    Vec3 size;
    Vec3 velocity;
    i32 isOverlapping;
};

struct EC_Renderer
{
    EntId entId;
    u8 inUse;
    RendObj rendObj;
};

struct EC_ActorMotor
{
    EntId entId;
    u8 inUse;
    Vec3 move;
    f32 speed;
};

struct EC_Projectile
{
    EntId entId;
    u8 inUse;
    Vec3 move;
    f32 speed;
    f32 tick;
    f32 tock;
};

//////////////////////////////////////////////////
// Define component list structs and GameState objects...
//////////////////////////////////////////////////
// GameState will require component lists to be defined already!
#include "game_entComponentBase.h"
DEFINE_ENT_COMPONENT_LIST(AIController)
DEFINE_ENT_COMPONENT_LIST(Renderer)
DEFINE_ENT_COMPONENT_LIST(Collider)
DEFINE_ENT_COMPONENT_LIST(ActorMotor)
DEFINE_ENT_COMPONENT_LIST(Projectile)

struct GameState
{
    u8 netMode; // 0 == server, 1 == client
    // Entities
    i32 nextEntityID;
    EntList entList;
    //i32 lastEntityIndex;

    // Components
    EC_AIControllerList aiControllerList;
    EC_RendererList rendererList;
    EC_ColliderList colliderList;
    EC_ActorMotorList actorMotorList;
    EC_ProjectileList projectileList;

    // specifics
    u16 playerEntityIndex;
};

//////////////////////////////////////////////////
// ...and Component Add/Remove/Has/Find functions.
// Note: requires that GameState struct is defined!
//////////////////////////////////////////////////
DEFINE_ENT_COMPONENT_BASE(AIController, aiController, COMP_FLAG_AICONTROLLER)
DEFINE_ENT_COMPONENT_BASE(Collider, collider, COMP_FLAG_COLLIDER)
DEFINE_ENT_COMPONENT_BASE(Renderer, renderer, COMP_FLAG_RENDERER)
DEFINE_ENT_COMPONENT_BASE(ActorMotor, actorMotor, COMP_FLAG_ACTORMOTOR)
DEFINE_ENT_COMPONENT_BASE(Projectile, projectile, COMP_FLAG_PROJECTILE)

// void World_Init(GameState* gs, i32 maxEntities)
// {
//     *gs = {};
//     gs->entities = entityList;
//     gs->maxEntities = maxEntities;
// }
