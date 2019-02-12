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
struct SimActorInput
{
    u32 buttons;
    Vec3 degrees;
};

#define SIM_ENT_STATUS_FREE 0
#define SIM_ENT_STATUS_RESERVED 1
#define SIM_ENT_STATUS_IN_USE 2

#define SIM_ENT_TYPE_NONE 0
#define SIM_ENT_TYPE_WORLD 1
#define SIM_ENT_TYPE_ACTOR 2
#define SIM_ENT_TYPE_PROJECTILE 3
#define SIM_ENT_TYPE_TURRET 4
#define SIM_ENT_TYPE_WANDERER 5 


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

struct SimEntity
{
    i32 status;
    i32 isLocal;
    SimEntId id;
    i32 entType;
	
	i32 fastForwardTicks;
    i32 birthTick;
    i32 deathTick;

    // timing
    f32 thinkTick;
    f32 thinkTime;
	f32 lifeTime;

    // physical
    Transform t;
    Vec3 previousPos;
    Vec3 velocity;
    f32 pitch;
    f32 yaw;
	
	SimActorInput input;
};

struct SimEntityDef
{
    i32 serial;

    i32 birthTick;
    //i32 deathTick;
    
    i32 isLocal;
    i32 entType;
    f32 pos[3];
    f32 scale[3];
    f32 velocity[3];
};

#define SIM_PROJ_TYPE_NONE 0
#define SIM_PROJ_TYPE_TEST 1
struct SimProjectileSpawnDef
{
    i32 firstSerial;
    i32 tick;
    u8 projType;
    u8 seedIndex;
    Vec3 pos;
	Vec3 forward;
};

struct SimEntBlock
{
    i32 index;
    SimEntity* ents;
    i32 capacity;
};

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

    // Command buffers
    //DoubleByteBuffer commands;
};

/*
struct SimEventHeader
{
	i32 type;
	i32 size;
	i32 tick;
	i32 sequence;
};

// look up data and write at transmission time
struct SimEventEntitySpawned
{
	SimEventHeader header;
	i32 serial;
};
*/
/*
Main replication events
> Entity is spawned.
> Pattern of entities is spawned.
> Entity is removed.
> Sync current Entity position

Questions:
> Where does game logic reside...? in the Simulation itself or the server/client?
> 
*/