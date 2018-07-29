#pragma once

#include "../common/com_module.h"

#define LOCAL_ENT_STATUS_FREE 0
#define LOCAL_ENT_STATUS_IN_USE 1

// Local entities are simple client side objects only and
// can have NO effect on server Ents whatsoever.
struct LocalEnt
{
    i32 id;
    i32 status;
    // movement
    Vec3 pos;
    Vec3 scale;
    Vec3 originalScale;
    Vec3 dir;
    f32 speed;

    // timing
    f32 tick;
    f32 tickMax;
    i32 tock;
    i32 tockMax;

    // display
    RendObj rend;
};

struct ClientTick
{
    //i32 mouse[2];
    //i32 mouseMovement[2];
    char attack1, attack2;
    char moveLeft, moveRight, moveUp, moveDown, moveForward, moveBackward;
    char yawLeft, yawRight, pitchUp, pitchDown;
    char rollLeft, rollRight;
    //char escape;
    //char reset;
    //InputItem debug_cycle;
    //char debug_break_game;

    Vec3 degrees = {};
};

union EntId;
struct Actor;

//////////////////////////////////////////////////
// Game Input
//////////////////////////////////////////////////
#define ACTOR_INPUT_MOVE_FORWARD (1 << 0)
#define ACTOR_INPUT_MOVE_BACKWARD (1 << 1)
#define ACTOR_INPUT_MOVE_LEFT (1 << 2)
#define ACTOR_INPUT_MOVE_RIGHT (1 << 3)
#define ACTOR_INPUT_MOVE_UP (1 << 4)
#define ACTOR_INPUT_MOVE_DOWN (1 << 5)
#define ACTOR_INPUT_ATTACK (1 << 6)
struct ActorInput
{
    u32 buttons;
    Vec3 degrees;
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
// NOTE: iteration starts at 1 and should never be zero
// An EntId of 0/0 is a 'NULL' entity
//////////////////////////////////////////////////
#pragma pack(push, 1)
union EntId
{
    struct
    {
        u16 iteration;
        u16 index;
    };
    u16 arr[2];
    u32 value;
};
#pragma pack(pop)

// Is this necessary?s
inline u8 EntId_Equals(EntId* a, EntId* b)
{
    return (a->index == b->index && a->iteration == b->iteration);
}

struct Ent
{
    // Instance identification
    EntId entId;            // iteration + index of this entity
    i32 tag;                // can (and should) be shared between entities for triggering

    EntId source;

    // 'type' information
    i32 factoryType;        // Spawn/Save/Sync function index used to generate this entity
    u32 componentFlags;     // What components this Entity has
    u8 inUse;               // if 0 this entity is free to be recycled
    u8 priority;            // This entity's base update importance gained per frame
};

struct EntList
{
    Ent* items;
    u32 count;
    u32 max;
};

struct EntityLink
{
    EntId entId;
    i32 importance;
    u8 priority;
    u8 relevant;
	// 16B + 4 + 1 + 1 = 22B
};

//////////////////////////////////////////////////
// Define Components
// All components required:
//   EntId entId;
//   u8 inUse;
//////////////////////////////////////////////////


struct Damager
{
    EntId source;
    i32 damage;
};

struct Ticker
{
    f32 tick;
    f32 tickMax;
    i32 tock;
    i32 tockMax;
};

// Max of 32 components per entity

//struct GameState;

struct EC_Header
{
    EntId entId;
    u8 inUse;
};

#define EC_FLAG_ENTITY (1 << 1)
#define EC_FLAG_TRANSFORM (1 << 2)
#define EC_FLAG_RENDERER (1 << 3)
#define EC_FLAG_COLLIDER (1 << 4)
#define EC_FLAG_AICONTROLLER (1 << 5)
#define EC_FLAG_ACTORMOTOR (1 << 6)
#define EC_FLAG_PROJECTILE (1 << 7)
#define EC_FLAG_LABEL (1 << 8)
#define EC_FLAG_HEALTH (1 << 9)
#define EC_FLAG_THINKER (1 << 10)

#define EC_NUM_TYPES 9

struct EC_Transform
{
    EC_Header header;

    Transform t;
};

#define EC_RENDERER_STRING_LENGTH 32

struct EC_RendererState
{
    char meshName[32];
    char textureName[32];

};

struct EC_Renderer
{
    EC_Header header;

    RendObj rendObj;

    EC_RendererState state;
};

struct EC_ColliderState
{
    ZShapeDef def;
    
    Vec3 size;
    Vec3 velocity;
};

struct EC_Collider
{
    EC_Header header;

    i32 shapeId;
    u32 lastFrameOverlapping;
    i32 isOverlapping;
    u8 isGrounded;

    EC_ColliderState state;
};

struct EC_ActorMotorState
{
    ActorInput input;
    Vec3 move;
    f32 runAcceleration;
    f32 runSpeed;
    Ticker ticker;
};

struct EC_ActorMotor
{
    EC_Header header;
    f32 debugCurrentSpeed;

    EC_ActorMotorState state;
};

// A quick test component
struct EC_AIController
{
    EC_Header header;

    Vec3 dir;
    f32 speed;
};

struct EC_ProjectileState
{
    Vec3 move;
    f32 speed;
    Ticker ticker;

    Damager damage;
};

struct EC_Projectile
{
    EC_Header header;

    EC_ProjectileState state;
};

#define EC_LABEL_LENGTH 32
struct EC_LabelState
{
    char label[EC_LABEL_LENGTH];
};

struct EC_Label
{
    EC_Header header;

    EC_LabelState state;
};

struct EC_HealthState
{
    i32 hp;
};

struct EC_Health
{
    EC_Header header;

    EC_HealthState state;
};

struct EC_ThinkerState
{
    Ticker ticker;
};

union Ent_Brain
{
    struct Spawner
    {
        i32 typeToSpawn;
    };
};

#define EC_BRAIN_NULL 0
#define EC_BRAIN_SPAWNER 1

struct EC_Thinker
{
    EC_Header header;

    Ticker ticker;

    i32 brainType;
    Ent_Brain brain;
};

/**
 * A master container for complete entity component state (spawning)
 * or pieces of component state (updating)
 */
struct EntityState
{
    EntId entId;
    // As this contains every possible state struct
    // use flags to know which should actually be used
    u32 componentBits;

    // keep in the same order or stuff will explode
    Ent entMetaData;
    Transform transform;
    EC_RendererState renderState;
    EC_ColliderState colliderState;
    EC_ActorMotorState actorState;
    EC_HealthState healthState;
    EC_ProjectileState projectileState;
    EC_LabelState labelState;
};

// Current things you can customise when you spawn an entity.
struct EntitySpawnOptions
{
    Vec3 pos;
    Vec3 rot;
    Vec3 scale;

    Vec3 vel;

    EntId source;
};

//////////////////////////////////////////////////
// Define component list structs and GameState objects...
//////////////////////////////////////////////////
// GameState will require component lists to be defined already!
#include "game_entComponentBase.h"
DEFINE_ENT_COMPONENT_LIST(Transform)
DEFINE_ENT_COMPONENT_LIST(Renderer)
DEFINE_ENT_COMPONENT_LIST(Collider)
DEFINE_ENT_COMPONENT_LIST(ActorMotor)
DEFINE_ENT_COMPONENT_LIST(AIController)
DEFINE_ENT_COMPONENT_LIST(Projectile)
DEFINE_ENT_COMPONENT_LIST(Label)
DEFINE_ENT_COMPONENT_LIST(Health)
DEFINE_ENT_COMPONENT_LIST(Thinker)

//////////////////////////////////////////////////
// Clients and players
//////////////////////////////////////////////////
struct Client
{
    i32 clientId;
    i32 state;
    EntId entId;
    i32 isLocal;
    ActorInput input;
};

struct ClientList
{
    Client* items;
    i32 count;
    i32 max;
};

// struct Player
// {
//     i32 playerId;
//     i32 state;
//     EntId entId;

//     // server only, not relicated
//     i32 sv_clientId;
// };

// struct PlayerList
// {
//     Player* items;
//     i32 count;
//     i32 max;
// };

//////////////////////////////////////////////////
// GameState God Object
// This is a very carefully implemented god object and you should all piss off
// Phil Bonneau (team Chef) 2018/6/26
//////////////////////////////////////////////////
struct GameState
{
    u8 netMode; // 0 == server, 1 == client
    
    //PlayerList playerList;
    ClientList clientList;

    // Entities
    //i32 nextEntityID;
    EntList entList;
    // Components
    EC_TransformList transformList;
    EC_RendererList rendererList;
    EC_ColliderList colliderList;
    EC_AIControllerList aiControllerList;
    EC_ActorMotorList actorMotorList;
    EC_ProjectileList projectileList;
    EC_LabelList labelList;
    EC_HealthList healthList;
    EC_ThinkerList thinkerList;
    
    // view
    Transform cameraTransform;
    
    // specifics
    u8 localPlayerHasEnt = 0;
    EntId localPlayerEntId;

    u16 debugMode = 0;
    char debugString[2048];
    u32 debugStringLength = 0;
    u32 debugStringCapacity = 2048;

    u8 verbose;
};

//////////////////////////////////////////////////
// ...and Component Add/Remove/Has/Find functions.
// Note: requires that GameState struct is defined!
//////////////////////////////////////////////////
DEFINE_ENT_COMPONENT_BASE(Transform, transform, EC_FLAG_TRANSFORM)
DEFINE_ENT_COMPONENT_BASE(Renderer, renderer, EC_FLAG_RENDERER)
DEFINE_ENT_COMPONENT_BASE(Collider, collider, EC_FLAG_COLLIDER)
DEFINE_ENT_COMPONENT_BASE(AIController, aiController, EC_FLAG_AICONTROLLER)
DEFINE_ENT_COMPONENT_BASE(ActorMotor, actorMotor, EC_FLAG_ACTORMOTOR)
DEFINE_ENT_COMPONENT_BASE(Projectile, projectile, EC_FLAG_PROJECTILE)
DEFINE_ENT_COMPONENT_BASE(Label, label, EC_FLAG_LABEL)
DEFINE_ENT_COMPONENT_BASE(Health, health, EC_FLAG_HEALTH)
DEFINE_ENT_COMPONENT_BASE(Thinker, thinker, EC_FLAG_THINKER)
