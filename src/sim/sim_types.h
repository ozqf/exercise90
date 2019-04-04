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

#define SIM_ENT_STATUS_FREE 0
#define SIM_ENT_STATUS_RESERVED 1
#define SIM_ENT_STATUS_IN_USE 2

#define SIM_ENT_TYPE_NONE 0
#define SIM_ENT_TYPE_WORLD 1
#define SIM_ENT_TYPE_ACTOR 2
#define SIM_ENT_TYPE_PROJECTILE 3
#define SIM_ENT_TYPE_TURRET 4
#define SIM_ENT_TYPE_WANDERER 5
#define SIM_ENT_TYPE_LINE_TRACE 6 


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
    i32 entType;
	
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

    i32 birthTick;
    //i32 deathTick;  // Die on a predetermined tick
    
    i32 isLocal;
    i32 entType;
    f32 pos[3];
    f32 scale[3];
    f32 velocity[3];
    Vec3 destination;
};

#define SIM_PROJ_TYPE_NONE 0
#define SIM_PROJ_TYPE_PLAYER_PREDICTION 1
#define SIM_PROJ_TYPE_TEST 2

struct SimSpawnDef
{
    Vec3 pos;
    Vec3 forward;
    i32 firstSerial;
    i32 tick;
};

struct SimProjectileSpawnDef
{
    SimSpawnDef base;
    u8 projType;
    u8 seedIndex;
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
