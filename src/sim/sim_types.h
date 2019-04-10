#pragma once

#include "../common/com_module.h"

#define ACTOR_INPUT_MOVE_FORWARD (1 << 0)
#define ACTOR_INPUT_MOVE_BACKWARD (1 << 1)
#define ACTOR_INPUT_MOVE_LEFT (1 << 2)
#define ACTOR_INPUT_MOVE_RIGHT (1 << 3)
#define ACTOR_INPUT_MOVE_UP (1 << 4)
#define ACTOR_INPUT_MOVE_DOWN (1 << 5)
#define ACTOR_INPUT_ATTACK (1 << 6)
#define ACTOR_INPUT_ATTACK2 (1 << 7)
#define ACTOR_INPUT_MOVE_SPECIAL1 (1 << 8)

#define ACTOR_INPUT_SHOOT_LEFT (1 << 27)
#define ACTOR_INPUT_SHOOT_RIGHT (1 << 28)
#define ACTOR_INPUT_SHOOT_UP (1 << 29)
#define ACTOR_INPUT_SHOOT_DOWN (1 << 30)
struct SimActorInput
{
    u32 buttons;
    Vec3 degrees;
};

#pragma pack(push, 1)
union SimEntIndex
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

struct SimEntId
{
    // location in entity blocks
    SimEntIndex slot;
    i32 serial;
};

struct SimEntDisplay
{
    Colour colour;
};

struct SimEntity
{
    i32 status;
    i32 isLocal;
    SimEntId id;

    simFactoryType factoryType;
    i32 tickType;

    // Relationships
    SimEntId targetId;  // current enemy
    SimEntId parentId;  // Who spawned this entity
    simFactoryType childFactoryType;
    i32 childCount;
    i32 maxChildCount;
	
	i32 fastForwardTicks;
    i32 birthTick;
    i32 deathTick;
    Vec3 destination;

    // timing
    f32 thinkTick;
    f32 thinkTime;
	f32 lifeTime;
    f32 attackTick;
    f32 attackTime;

    SimEntDisplay display;

    u32 flags;

    // physical
    Transform t;
    Vec3 previousPos;
    f32 speed;
    Vec3 velocity;
    f32 pitch;
    f32 yaw;
	
	SimActorInput input;

    // runtime
    i32 hasBeenPredicted;
};

struct SimEntityDef
{
    i32 serial;
    i32 fastForwardTicks;

    i32 birthTick;
    
    i32 isLocal;
    simFactoryType factoryType;
    simFactoryType childFactoryType;

    Vec3 pos;
    Vec3 scale;
    Vec3 velocity;
    Vec3 destination;
};

struct SimSpawnPatternItem
{
	i32 entSerial;
	Vec3 pos;
	Vec3 forward;
};

struct SimSpawnPatternDef
{
	i32 patternId;
	i32 numItems;
	f32 radius;
};

struct SimProjectileType
{
    // Characterics of each projectile
    f32 speed;
    f32 lifeTime;
    f32 arcDegrees;

    ColourU32 colour;
    Vec3 scale;

    // Characterics of how this projectile is spawned
    SimSpawnPatternDef patternDef;
};

struct SimSpawnBase
{
    Vec3 pos;
    Vec3 forward;
    i32 firstSerial;
    i32 tick;
    u8 seedIndex;
};

struct SimBulkSpawnEvent
{
    SimSpawnBase base;
    SimSpawnPatternDef patternDef;
    u8 factoryType;
};

#if 0
// For block allocated entity storage
struct SimEntBlock
{
    i32 index;
    SimEntity* ents;
    i32 capacity;
};
#endif
struct SimScene
{
    SimEntity* ents;
    i32 maxEnts;

    // sequential, unrelated to blocks
    i32 remoteEntitySequence;
    i32 localEntitySequence;
    i32 cmdSequence;
    u32 tick;

    Vec3 boundaryMin;
    Vec3 boundaryMax;
};
