#pragma once

#include "../../common/common.h"

////////////////////////////////////////////////////////////
// ENTITY TEMPLATE TYPES
////////////////////////////////////////////////////////////
#define ENTITY_TYPE0_NULL 0
#define ENTITY_TYPE1_WORLD_CUBE 1
#define ENTITY_TYPE2_RIGIDBODY_CUBE 2
#define ENTITY_TYPE3_ACTOR_GROUND 3
#define ENTITY_TYPE4_PROJECTILE 4
#define ENTITY_TYPE5_SPAWNER 5
#define ENTITY_TYPE6_ENEMY 6
#define ENTITY_TYPE7_BLOCKING_VOLUME 7
#define ENTITY_TYPE8_ENEMY_BRUTE 8
#define ENTITY_TYPE9_ENEMY_CHARGER 9
#define ENTITY_TYPE10_ENEMY_FODDER 10
#define ENTITY_TYPE11_ENEMY_SWARM 11
#define ENTITY_TYPE12_ENEMY_SPINNER 12
#define ENTITY_TYPE13_VOLUME 13
#define ENTITY_TYPE14_ACID 14
#define ENTITY_TYPE15_LAVA 15

////////////////////////////////////////////////////////////
// ENTITY CATAGORIES
////////////////////////////////////////////////////////////
#define ENTITY_CATAGORY_NULL 0
#define ENTITY_CATAGORY_REPLICATED 1
#define ENTITY_CATAGORY_LOCAL 2

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
struct LocalEnt;
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
#define ACTOR_INPUT_MOVE_SPECIAL1 (1 << 8)
struct ActorInput;
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
#define ENT_ID_NULL 0

union EntId;
// If entities are assigned in blocks (an array of fixed size arrays) this is how to find them
//#define ENTID_TO_ENT_BLOCK(entityId, blockSize) ((i32)(entityId.index / blockSize))
//#define ENTID_TO_ENT_INDEX(entityId, blockSize) ((i32)(entityId.index % blockSize))

// Entities are defined by the components grouped by ent id.
// consider this struct as entity 'meta data'
struct Ent;
struct EntList;
struct EntityLink;
#define ATTACK_INDEX_NULL 0
#define ATTACK_INDEX_FAST_PROJECTILE 1
#define ATTACK_INDEX_SLOW_PROJECTILE 2
#define ATTACK_INDEX_PROJECTILE_SHOTGUN 3

struct AttackInfo;
struct Damager;
struct Ticker;
struct UIEntity;
struct HudRingItem;
//////////////////////////////////////////////////
// Define Components
// All components required:
//   EntId entId;
//   u8 inUse;
//////////////////////////////////////////////////

// Max of 32 component TYPES

#define EC_GET_ID(ec) ( ec##->header.entId )

struct EC_Header;

#define EC_BIT0_ENTITY (1 << 0)
#define EC_BIT1_TRANSFORM (1 << 1)
#define EC_BIT2_RENDERER (1 << 2)
#define EC_BIT3_COLLIDER (1 << 3)
#define EC_BIT4_AICONTROLLER (1 << 4)
#define EC_BIT5_ACTORMOTOR (1 << 5)
#define EC_BIT6_PROJECTILE (1 << 6)
#define EC_BIT7_LABEL (1 << 7)
#define EC_BIT8_HEALTH (1 << 8)
#define EC_BIT9_THINKER (1 << 9)
#define EC_BIT10_MULTI_RENDOBJ (1 << 10)
#define EC_BIT11_VOLUME (1 << 11)
#define EC_BIT12_SENSOR (1 << 12)

// All entity component state should have a flags member
// the first bit is always 0 for active, 1 for inactive
// IMPORTANT AND DODGY:
// these bits are therefore RESERVED and must not be
// override with the component's own settings
#define EC_STATE_FLAG_INACTIVE (1 << 0)
#define EC_STATE_FLAG_IS_PLAYER (1 << 1)

//#define EC_NUM_TYPES 9

// TODO: Remove me, I'm here because of laziness
struct EC_TransformState;
struct EC_Transform;
///////////////////////////////////////////////////////
//  RENDER COMPONENTS
///////////////////////////////////////////////////////
#define EC_RENDERER_STRING_LENGTH 32

#define EC_RENDOBJ_ANGLE_MODE_DEFAULT 0
#define EC_RENDOBJ_ANGLE_MODE_BIPED 1

struct EC_RendObjState;
struct EC_SingleRendObj;
#define EC_MULTI_RENDERER_CAPACITY 2
#define EC_MULTI_RENDOBJ_BASE 0
#define EC_MULTI_RENDOBJ_HEAD 1
struct EC_MultiRendObjState;
struct EC_MultiRendObj;
struct EC_ColliderState;
struct EC_Collider;
#define EC_ACTOR_MOTOR_FLAG_MOVE_SPECIAL_LOCKED (1 << 31)

struct EC_ActorMotorState;
struct EC_ActorMotor;
#define ATTACK_FLAG_NO_TARGET_TRACK (1 << 0)
#define ATTACK_FLAG_BREAK_ON_LOS_LOSS (1 << 1)

struct AI_AttackSettings;
struct EC_AIState;
// A quick test component
struct EC_AIController;
struct EC_ProjectileState;
struct EC_Projectile;
#define EC_LABEL_LENGTH 32
struct EC_LabelState;
struct EC_Label;
#define EC_HEALTH_FLAG_STUN_IMMUNE (1 << 1)
#define EC_HEALTH_FLAG_INVULNERABLE (1 << 2)

struct EC_HealthState;
struct EC_Health;
struct EC_ThinkerState;
#define EC_THINKER_NULL 0
#define EC_THINKER_SPAWNER 1

struct EC_Thinker;
#define EC_VOLUME_TYPE_TRIGGER 0
#define EC_VOLUME_TYPE_ACID 1
#define EC_VOLUME_TYPE_LAVA 2
#define EC_VOLUME_TYPE_PROTECTION 3
#define EC_VOLUME_TYPE_POWER 4

struct EC_VolumeState;
struct EC_Volume;
#define EC_SENSOR_FLAG_DOES_DAMAGE (1 << 31)

struct EC_SensorState;
struct EC_Sensor;
/**
 * A master container for complete entity component state (spawning)
 * or pieces of component state (updating)
 */
struct EntityState;
#define ENT_OPTION_FLAG_SCALE (1 << 1)
#define ENT_OPTION_FLAG_TEAM (1 << 2)
#define ENT_OPTION_FLAG_VELOCITY (1 << 3)
// Current things you can customise when you spawn an entity.
struct EntitySpawnOptions;
//////////////////////////////////////////////////
// Clients and players
//////////////////////////////////////////////////
#define CLIENT_STATE_FREE 0
#define CLIENT_STATE_OBSERVER 1
#define CLIENT_STATE_PLAYING 2
#define CLIENT_STATE_SYNC_LEVEL 3
#define CLIENT_STATE_SYNC_ENTITIES 4


#define CLIENT_FLAG_LOCAL (1 << 0)
struct Client;
struct ClientList;
///////////////////////////////////////////////////////////////////////
// Session data
///////////////////////////////////////////////////////////////////////

#define GAME_SESSION_STATE_WARMUP 0
#define GAME_SESSION_STATE_PLAYING 1
#define GAME_SESSION_STATE_FAILED 2
#define GAME_SESSION_STATE_SUCCESS 3
struct GameSession;
#define IS_SERVER (IsRunningServer((GetSession())->netMode))
#define IS_CLIENT (IsRunningClient((GetSession())->netMode))
struct EntityBlock;
struct GameScene;
struct Game;
