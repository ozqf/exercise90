#pragma once

#include "../common/com_module.h"

////////////////////////////////////////////////////////////
// ENTITY TEMPLATE TYPES
////////////////////////////////////////////////////////////
#define ENTITY_TYPE_NULL 0
#define ENTITY_TYPE_WORLD_CUBE 1
#define ENTITY_TYPE_RIGIDBODY_CUBE 2
#define ENTITY_TYPE_ACTOR_GROUND 3
#define ENTITY_TYPE_PROJECTILE 4
#define ENTITY_TYPE_SPAWNER 5
#define ENTITY_TYPE_ENEMY 6
#define ENTITY_TYPE_BLOCKING_VOLUME 7
#define ENTITY_TYPE_ENEMY_BRUTE 8
#define ENTITY_TYPE_ENEMY_CHARGER 9
#define ENTITY_TYPE_ENEMY_FODDER 10
#define ENTITY_TYPE_ENEMY_SWARM 11
#define ENTITY_TYPE_ENEMY_SPINNER 12

////////////////////////////////////////////////////////////
// LOCAL ENTITY IDS
////////////////////////////////////////////////////////////
#define LOCAL_ENT_TYPE_NULL 0
#define LOCAL_ENT_TYPE_IMPACT 1
#define LOCAL_ENT_TYPE_SPAWN 2
#define LOCAL_ENT_TYPE_EXPLOSION 3
#define LOCAL_ENT_TYPE_DEBRIS 4

#define LOCAL_ENT_STATUS_FREE 0
#define LOCAL_ENT_STATUS_IN_USE 1

#define LOCAL_ENT_UPDATER_DEFAULT 0
#define LOCAL_ENT_UPDATER_DYNAMIC 1
#define LOCAL_ENT_UPDATER_STATIC 2
#define LOCAL_ENT_UPDATER_PHYSICS 3

#define LOCAL_ENT_FLAG_PHYSICS (1 << 1)

// Local entities are simple client side objects only and
// can have NO effect on server Ents whatsoever.
struct LocalEnt
{
    u32 id;
    i32 status;
	
    // movement
    Vec3 pos;
	Vec3 startPos;
	Vec3 endPos;
    Vec3 rotationDegrees;
	
    Vec3 scale;
    Vec3 originalScale;
    Vec3 targetScale;
	
	Colour colour;
	Colour startColour;
	Colour endColour;
	
    Vec3 dir;
    f32 speed;

    i32 updaterIndex;
    u32 flags;
    i32 shapeId;

    // timing
    f32 tick;
    f32 tickMax;
	f32 timeToLive;

    // display
    RendObj rend;
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
#define ACTOR_INPUT_ATTACK2 (1 << 7)
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

// Is this necessary?
// TODO: No it isn't. Compare via id.value is much easier
inline u8 EntId_Equals(EntId* a, EntId* b)
{
    return (a->index == b->index && a->iteration == b->iteration);
}

// Entities are defined by the components grouped by ent id.
// consider this struct as entity 'meta data'
struct Ent
{
    // Instance identification
    EntId entId;            // iteration + index of this entity
    i32 tag;                // can (and should) be shared between entities for triggering
    
	i32 team;
    EntId source;           // id of whatever spawned/shot this ent

    // 'type' information
    i32 factoryType;        // Spawn/Save/Sync function index used to generate this entity
    u32 componentBits;      // What components this Entity has
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

#define ATTACK_INDEX_NULL 0
#define ATTACK_INDEX_FAST_PROJECTILE 1
#define ATTACK_INDEX_SLOW_PROJECTILE 2
#define ATTACK_INDEX_PROJECTILE_SHOTGUN 3

struct AttackInfo
{
    i32 type;
    Vec3 origin;
    EntId source;
    i32 team;
    f32 yawDegrees;
    f32 pitchDegrees;
};


struct Damager
{
    EntId source;
    i32 damage;
};

struct Ticker
{
    f32 tick;
    f32 tickMax;
    //i32 tock;
    //i32 tockMax;
};

//////////////////////////////////////////////////
// Define Components
// All components required:
//   EntId entId;
//   u8 inUse;
//////////////////////////////////////////////////

// Max of 32 component TYPES

#define EC_GET_ID(ec) ( ec##->header.entId )

struct EC_Header
{
    EntId entId;
    u8 inUse;
};

#define EC_FLAG_ENTITY (1 << 0)
#define EC_FLAG_TRANSFORM (1 << 1)
#define EC_FLAG_RENDERER (1 << 2)
#define EC_FLAG_COLLIDER (1 << 3)
#define EC_FLAG_AICONTROLLER (1 << 4)
#define EC_FLAG_ACTORMOTOR (1 << 5)
#define EC_FLAG_PROJECTILE (1 << 6)
#define EC_FLAG_LABEL (1 << 7)
#define EC_FLAG_HEALTH (1 << 8)
#define EC_FLAG_THINKER (1 << 9)
#define EC_FLAG_MULTI_RENDOBJ (1 << 10)
#define EC_FLAG_VOLUME (1 << 11)

// All entity component state should have a flags member
// the first bit is always 0 for active, 1 for inactive
#define EC_STATE_FLAG_INACTIVE (1 << 0)
#define EC_STATE_FLAG_IS_PLAYER (1 << 1)

//#define EC_NUM_TYPES 9

// TODO: Remove me, I'm here because of laziness
struct EC_TransformState
{
	u32 flags;
}; 

struct EC_Transform
{
    EC_Header header;
	EC_TransformState state;
    Transform t;
};

///////////////////////////////////////////////////////
//  RENDER COMPONENTS
///////////////////////////////////////////////////////
#define EC_RENDERER_STRING_LENGTH 32

#define EC_RENDOBJ_ANGLE_MODE_DEFAULT 0
#define EC_RENDOBJ_ANGLE_MODE_BIPED 1

#if 1 // Single mesh renderer component
struct EC_RendObjState
{
	u32 flags;
    char meshName[EC_RENDERER_STRING_LENGTH];
    char textureName[EC_RENDERER_STRING_LENGTH];
    f32 colourRGB[3];
	i32 flashFrames;
    f32 pitchDegrees;
    f32 yawDegrees;
    f32 heightOffset;
    u8 mode;
};

struct EC_SingleRendObj
{
    EC_Header header;

    RendObj rendObj;

    EC_RendObjState state;
};
#endif

#define EC_MULTI_RENDERER_CAPACITY 2
#define EC_MULTI_RENDOBJ_BASE 0
#define EC_MULTI_RENDOBJ_HEAD 1
#if 1
struct EC_MultiRendObjState
{
	u32 flags;
    EC_RendObjState objStates[EC_MULTI_RENDERER_CAPACITY];

    EC_RendObjState* GetBaseRendObj() { return &this->objStates[EC_MULTI_RENDOBJ_BASE]; }
    EC_RendObjState* GetHeadRendObj() { return &this->objStates[EC_MULTI_RENDOBJ_HEAD]; }
};

struct EC_MultiRendObj
{
    EC_Header header;

    RendObj rendObjs[EC_MULTI_RENDERER_CAPACITY];
    EC_MultiRendObjState state;
};
#endif


// #define EC_COLLIDER_HANDLER_NONE 0
// #define EC_COLLIDER_HANDLER_AI 1
// #define EC_COLLIDER_HANDLER_HEALTH 1

struct EC_ColliderState
{
	u32 flags;
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
	u32 flags;
    ActorInput input;
    Vec3 move;
    f32 runAcceleration;
    f32 runSpeed;
	
    i32 attack1Type;
	f32 attack1Reload;
	f32 attack1AnimStyle;
	
	i32 attack2Type;
	f32 attack2Reload;
	f32 attack2AnimStyle;
    Ticker ticker;
};

struct EC_ActorMotor
{
    EC_Header header;
    f32 debugCurrentSpeed;

    EC_ActorMotorState state;
};

#define ATTACK_FLAG_NO_TARGET_TRACK (1 << 0)
#define ATTACK_FLAG_BREAK_ON_LOS_LOSS (1 << 1)

struct AI_AttackSettings
{
    i32 type;
    f32 startUpDelay;
    f32 repeatDelay;
    i32 repeatCount;
    f32 recoverDelay;
    u32 flags;

    void Set(
        i32 newType,
        f32 newStartDelay,
        f32 newRepeatDelay,
        i32 newRepeatCount,
        f32 newRecoverDelay,
        u32 newFlags)
    {
        this->type = newType;
        this->startUpDelay = newStartDelay;
        this->repeatDelay = newRepeatDelay;
        this->repeatCount = newRepeatCount;
        this->recoverDelay = newRecoverDelay;
        this->flags = newFlags;
    }
};

struct EC_AIState
{
	u32 flags;
    EntId target;
    i32 state;
    i32 type;
    Vec3 dir;
    f32 speed;
    f32 minApproachDistance;
    f32 nextMoveCalc;
    f32 nextThink;
    i32 attackCount;

    AI_AttackSettings atkSettings;
};

// A quick test component
struct EC_AIController
{
    EC_Header header;

    EC_AIState state;
};

struct EC_ProjectileState
{
	u32 flags;
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
	u32 flags;
    char label[EC_LABEL_LENGTH];
};

struct EC_Label
{
    EC_Header header;

    EC_LabelState state;
};

#define EC_HEALTH_FLAG_STUN_IMMUNE (1 << 1)
#define EC_HEALTH_FLAG_INVULNERABLE (1 << 2)

struct EC_HealthState
{
	u32 flags;
    i32 hp;
    i32 maxHp;
    u32 team;
    i32 damageThisFrame;
    i32 stunThreshold;
	u32 lastHitFrame;
	EntId lastHitSource;


    void SetHealthAndMax(i32 max)
    {
        this->maxHp = max;
        this->hp = max;
    }
};

struct EC_Health
{
    EC_Header header;
    
    EC_HealthState state;
};

struct EC_ThinkerState
{
	u32 flags;
    i32 type;
    Ticker ticker;
    i32 count;
    i32 max;
};

#define EC_THINKER_NULL 0
#define EC_THINKER_SPAWNER 1

struct EC_Thinker
{
    EC_Header header;

    EC_ThinkerState state;
};

#define EC_VOLUME_TYPE_NONE 0
#define EC_VOLUME_TYPE_DAMAGE 1	// hurt teammates overlapping
#define EC_VOLUME_TYPE_HEAL 2	// heal teammates overlapping

struct EC_VolumeState
{
	u32 flags;
	i32 type;
	Vec3 halfSize;
};

struct EC_Volume
{
	EC_Header header;
	
	EC_VolumeState state;
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
    EC_RendObjState renderState;
    EC_ColliderState colliderState;
    EC_AIState aiState;
    EC_ActorMotorState actorState;
    EC_HealthState healthState;
    EC_ProjectileState projectileState;
    EC_LabelState labelState;
    EC_ThinkerState thinkerState;
    EC_MultiRendObjState multiRendState;
	EC_VolumeState volumeState;
};

#define ENT_OPTION_FLAG_SCALE (1 << 1)
#define ENT_OPTION_FLAG_TEAM (1 << 2)
// Current things you can customise when you spawn an entity.
struct EntitySpawnOptions
{
    u32 flags;
    Vec3 pos;
    Vec3 rot;
    Vec3 scale;

    Vec3 vel;

    EntId source;
    i32 team;
};

//////////////////////////////////////////////////
// Define component list structs and GameState objects...
//////////////////////////////////////////////////
// GameState will require component lists to be defined already!
#include "game_entComponentBase.h"
DEFINE_ENT_COMPONENT_LIST(Transform)
DEFINE_ENT_COMPONENT_LIST(SingleRendObj)
DEFINE_ENT_COMPONENT_LIST(Collider)
DEFINE_ENT_COMPONENT_LIST(ActorMotor)
DEFINE_ENT_COMPONENT_LIST(AIController)
DEFINE_ENT_COMPONENT_LIST(Projectile)
DEFINE_ENT_COMPONENT_LIST(Label)
DEFINE_ENT_COMPONENT_LIST(Health)
DEFINE_ENT_COMPONENT_LIST(Thinker)
DEFINE_ENT_COMPONENT_LIST(MultiRendObj)

//////////////////////////////////////////////////
// Clients and players
//////////////////////////////////////////////////
#define CLIENT_STATE_FREE 0
#define CLIENT_STATE_OBSERVER 1
#define CLIENT_STATE_PLAYING 2
struct Client
{
    i32 clientId;
    i32 state;
    EntId entId;
    i32 isLocal;
    ActorInput input;

    u8 IsPlaying() { return (this->state == CLIENT_STATE_PLAYING); }
};

struct ClientList
{
    Client* items;
    i32 count;
    i32 max;
};


///////////////////////////////////////////////////////////////////////
// Session data
///////////////////////////////////////////////////////////////////////

// A Campaign spans multiple sessions
// Stores the starting conditions of the current game session and
// results of previous sessions
// From hitting start in the main menu to game over
struct GameCampaign
{
    char mapName[64];
    i32 levelsCompleted = 0;
};

#define GAME_SESSION_STATE_WARMUP 0
#define GAME_SESSION_STATE_PLAYING 1
#define GAME_SESSION_STATE_FAILED 2
#define GAME_SESSION_STATE_SUCCESS 3

// overall game information in the current instance (eg progress, wave count, scores etc)
struct GameSession
{
    i32 score;
    i32 state;
	
	u8 AllowPlayerSpawning()
	{
		return (this->state == GAME_SESSION_STATE_PLAYING
		|| this->state == GAME_SESSION_STATE_WARMUP);
	}
};

// Specific local settings on this machine
struct GameStateLocal
{
    u8 localPlayerHasEnt = 0;
    EntId localPlayerEntId;
};

#define IS_SERVER(ptrGameState) (IsRunningServer(##ptrGameState##->netMode))

//////////////////////////////////////////////////
// GameState God Object
// This is a very carefully implemented god object and you should all piss off
// Phil Bonneau (team Chef) 2018/6/26
//////////////////////////////////////////////////
struct GameState
{
    u8 netMode; // 0 == server, 1 == client
    
    GameCampaign campaign;
    GameSession session;
    
    ClientList clientList;
    GameStateLocal local;

    // Entities
    //i32 nextEntityID;
    EntList entList;
    // Components
    EC_TransformList transformList;
    EC_SingleRendObjList singleRendObjList;
    EC_ColliderList colliderList;
    EC_AIControllerList aiControllerList;
    EC_ActorMotorList actorMotorList;
    EC_ProjectileList projectileList;
    EC_LabelList labelList;
    EC_HealthList healthList;
    EC_ThinkerList thinkerList;
    EC_MultiRendObjList multiRendObjList;
    
    // view
    Transform cameraTransform;
    EntId cameraEntId;
    
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
DEFINE_ENT_COMPONENT_BASE(SingleRendObj, singleRendObj, EC_FLAG_RENDERER)
DEFINE_ENT_COMPONENT_BASE(Collider, collider, EC_FLAG_COLLIDER)
DEFINE_ENT_COMPONENT_BASE(AIController, aiController, EC_FLAG_AICONTROLLER)
DEFINE_ENT_COMPONENT_BASE(ActorMotor, actorMotor, EC_FLAG_ACTORMOTOR)
DEFINE_ENT_COMPONENT_BASE(Projectile, projectile, EC_FLAG_PROJECTILE)
DEFINE_ENT_COMPONENT_BASE(Label, label, EC_FLAG_LABEL)
DEFINE_ENT_COMPONENT_BASE(Health, health, EC_FLAG_HEALTH)
DEFINE_ENT_COMPONENT_BASE(Thinker, thinker, EC_FLAG_THINKER)
DEFINE_ENT_COMPONENT_BASE(MultiRendObj, multiRendObj, EC_FLAG_MULTI_RENDOBJ)
