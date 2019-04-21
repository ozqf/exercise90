#pragma once

#include "../../common/common.h"
#include "../app_network_types.h"
#include "game_entComponentBase.h"

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

// If entities are assigned in blocks (an array of fixed size arrays) this is how to find them
//#define ENTID_TO_ENT_BLOCK(entityId, blockSize) ((i32)(entityId.index / blockSize))
//#define ENTID_TO_ENT_INDEX(entityId, blockSize) ((i32)(entityId.index % blockSize))

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
};

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

////////////////////////////////////////////////////////////////////
// UI
////////////////////////////////////////////////////////////////////
struct UIEntity
{
    i32 id;
    u8 inUse;
    u8 isInteractive;
    i32 state;
    Transform transform;
    f32 halfWidth;
    f32 halfHeight;
    RendObj rendObj;
	RendObj debugRend;		// for drawing collision boxes
    char name[16];
};

struct HudRingItem
{
    i32 state;
    f32 radius;
    Vec3 screenPos;
    f32 scale;

    f32 startScale;
    f32 endScale;
    f32 tick;
    f32 tickMax;
    
    f32 degrees;
    Vec3 worldPos;

    RendObj rendObj;
};

//////////////////////////////////////////////////
// Define Components
// All components required:
//   EntId entId;
//   u8 inUse;
//////////////////////////////////////////////////

// Max of 32 component TYPES

struct EC_Header
{
    EntId entId;
    u8 inUse;
};

// All entity component state should have a flags member
// the first bit is always 0 for active, 1 for inactive
// IMPORTANT AND DODGY:
// these bits are therefore RESERVED and must not be
// override with the component's own settings

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
    i32 canUseMove;

    f32 specialRechargeTime;
	
    i32 attack1Type;
	f32 attack1Reload;
	i32 attack1AnimStyle;
	
	i32 attack2Type;
	f32 attack2Reload;
	i32 attack2AnimStyle;
    
    i32 animStyle;
    Ticker ticker;
};

struct EC_ActorMotor
{
    EC_Header header;
    f32 debugCurrentSpeed;

    EC_ActorMotorState state;
};

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
struct EC_Thinker
{
    EC_Header header;

    EC_ThinkerState state;
};
struct EC_VolumeState
{
	u32 flags;
	i32 type;
	Vec3 halfSize;
	i32 moveType;
};

struct EC_Volume
{
	EC_Header header;
	
	EC_VolumeState state;
};
struct EC_SensorState
{
    u32 flags;
	// bits for components that are interested in collisions
    u32 listenerComponents;
};

struct EC_Sensor
{
    EC_Header header;
    EC_SensorState state;
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
    EC_SensorState sensorState;
};
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
// Define component list structs and GameScene objects...
//////////////////////////////////////////////////
// GameScene will require component lists to be defined already!

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
DEFINE_ENT_COMPONENT_LIST(Volume)
DEFINE_ENT_COMPONENT_LIST(Sensor)

//////////////////////////////////////////////////
// Clients and players
//////////////////////////////////////////////////
struct Client
{
    // private between client and server
    i32 connectionId;
    // public identity for client to client and local application
    i32 clientId;
    i32 state;
	u32 flags;
    EntId entId;
    i32 isLocal;
    ActorInput input;

    NetStream stream;
    // Clients are considered in sync mode until their acknowledged
    // reliable message queue Id is >= to this.
    // Set to u32 max value when client first connects.
    // Set properly after all sync messages have been buffered for 
    // transmission.
    u32 syncCompleteMessageId;

    u8 IsPlaying() { return (this->state == CLIENT_STATE_PLAYING); }
};

struct ClientList
{
    Client* items;
    i32 count;
    i32 max;
    i32 nextId;
    // id of the client this exe represents
    // 0 == no local client
    i32 localClientId = 0;
};


///////////////////////////////////////////////////////////////////////
// Session data
///////////////////////////////////////////////////////////////////////

// Stores the starting conditions of the current game session and
// results of previous sessions
// From hitting start in the main menu to game over
struct GameSession
{
    u8 netMode; // 0 == server, 1 == client
    //i32 score;
    //i32 state;
    char mapName[64];
    i32 levelsCompleted = 0;
    //i32 localClientId = 0;
    
    ClientList clientList;
    i32 remoteConnectionId;
};

// Specific local settings on this machine
// struct GameSceneLocal
// {
//     u8 localPlayerHasEnt = 0;
//     EntId localPlayerEntId;
// };

#if 0
// meh, gone off this idea for now
struct EntityType
{
    i32 id;
    char* name;

    EntId (*Spawn)(GameScene* scene);
    i32 (*WriteNetworkSync)(GameScene* scene, EntId id, u8* ptr);
    i32 (*ReadNetworkSync)(GameScene* scene, EntId id, u8* ptr);
    i32 (*SaveState)(u8* ptr);
    i32 (*LoadState)(u8* ptr);
};
#endif
struct EntityBlock
{
    i32 isAllocated;

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
    EC_VolumeList volumeList;
    EC_SensorList sensorList;
};

//////////////////////////////////////////////////
// GameScene God Object
// This is a very carefully implemented god object and you should all piss off
// Phil Bonneau (team Chef) 2018/6/26
//////////////////////////////////////////////////
struct GameScene
{
    i32 state;

    EntityBlock ents[8];
    
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
    EC_VolumeList volumeList;
    EC_SensorList sensorList;
    
    // view
    Transform cameraTransform;
    EntId cameraEntId;
    
    u16 debugMode = 7;
    char debugString[2048];
    u32 debugStringLength = 0;
    u32 debugStringCapacity = 2048;

    i32 verboseFramesTick = 0;
    i32 alwaysVerbose;

    i32 Verbose()
    {
        return (verboseFramesTick || alwaysVerbose);
    }

    u8 AllowPlayerSpawning()
	{
		return (this->state == GAME_SESSION_STATE_PLAYING
		|| this->state == GAME_SESSION_STATE_WARMUP);
	}
};

//////////////////////////////////////////////////
// ...and Component Add/Remove/Has/Find functions.
// Note: requires that GameScene struct is defined!
//////////////////////////////////////////////////
DEFINE_ENT_COMPONENT_BASE(Transform, transform, EC_BIT1_TRANSFORM)
DEFINE_ENT_COMPONENT_BASE(SingleRendObj, singleRendObj, EC_BIT2_RENDERER)
DEFINE_ENT_COMPONENT_BASE(Collider, collider, EC_BIT3_COLLIDER)
DEFINE_ENT_COMPONENT_BASE(AIController, aiController, EC_BIT4_AICONTROLLER)
DEFINE_ENT_COMPONENT_BASE(ActorMotor, actorMotor, EC_BIT5_ACTORMOTOR)
DEFINE_ENT_COMPONENT_BASE(Projectile, projectile, EC_BIT6_PROJECTILE)
DEFINE_ENT_COMPONENT_BASE(Label, label, EC_BIT7_LABEL)
DEFINE_ENT_COMPONENT_BASE(Health, health, EC_BIT8_HEALTH)
DEFINE_ENT_COMPONENT_BASE(Thinker, thinker, EC_BIT9_THINKER)
DEFINE_ENT_COMPONENT_BASE(MultiRendObj, multiRendObj, EC_BIT10_MULTI_RENDOBJ)
DEFINE_ENT_COMPONENT_BASE(Volume, volume, EC_BIT11_VOLUME)
DEFINE_ENT_COMPONENT_BASE(Sensor, sensor, EC_BIT12_SENSOR)

struct Game
{
    // TODO Change names to ptrScene/ptrSession to make it more
    // obvious they're that they're not members
    GameSession* session;
    GameScene* scene;
};
