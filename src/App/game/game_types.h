#pragma once

#include "../common/com_module.h"

struct ClientTick
{
    i32 mouse[2];
    i32 mouseMovement[2];
    char attack1, attack2;
    char moveLeft, moveRight, moveUp, moveDown, moveForward, moveBackward;
    char yawLeft, yawRight, pitchUp, pitchDown;
    char rollLeft, rollRight;
    char escape;
    char reset;
    InputItem debug_cycle;
    char debug_break_game;

    Vec3 degrees = {};
};

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
#define COMP_FLAG_LABEL (1 << 5)

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

#define ACTOR_INPUT_MOVE_FORWARD (1 << 0)
#define ACTOR_INPUT_MOVE_BACKWARD (1 << 1)
#define ACTOR_INPUT_MOVE_LEFT (1 << 2)
#define ACTOR_INPUT_MOVE_RIGHT (1 << 3)
struct ActorInput
{
    u32 inputs;
    Vec3 degrees;
};

struct EC_ActorMotor
{
    EntId entId;
    u8 inUse;
    ActorInput input;
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

struct EC_Label
{
    EntId entId;
    u8 inUse;
    char label[32];
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
DEFINE_ENT_COMPONENT_LIST(Label)

struct Player
{
    i32 id;
    i32 state;
    ActorInput input;
};

struct GameState
{
    u8 netMode; // 0 == server, 1 == client
    // Entities
    i32 nextEntityID;
    EntList entList;
    //i32 lastEntityIndex;

    Player players[4];
    i32 numPlayers = 0;
    i32 maxPlayers = 4;

    // Components
    EC_AIControllerList aiControllerList;
    EC_RendererList rendererList;
    EC_ColliderList colliderList;
    EC_ActorMotorList actorMotorList;
    EC_ProjectileList projectileList;
    EC_LabelList labelList;
    
    // view
    Transform cameraTransform;
    //AngleVectors cameraAngleVectors;

    // specifics
    u16 playerEntityIndex;

    u16 debugMode = 2;
    char debugString[2048];
    u32 debugStringLength = 0;
    u32 debugStringCapacity = 2048;
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
DEFINE_ENT_COMPONENT_BASE(Label, label, COMP_FLAG_LABEL)
