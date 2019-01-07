#pragma once

#include "../common/com_module.h"
/*
#define SIM_ENT_BLOCK_SIZE 1024
#define SIM_ENT_MAX_BLOCKS 128
// serial numbers for entities.
#define SIM_REPLICATED_ENT_SERIAL_MIN 0
#define SIM_REPLICATED_ENT_SERIAL_MAX 1073741822
#define SIM_LOCAL_ENT_SERIAL_MIN 1073741823
#define SIM_LOCAL_ENT_SERIAL_MAX 2147483647
*/
#define SIM_ENT_STATUS_FREE 0
#define SIM_ENT_STATUS_RESERVED 1
#define SIM_ENT_STATUS_IN_USE 2

#define SIM_ENT_TYPE_NONE 0
#define SIM_ENT_TYPE_WORLD 1
#define SIM_ENT_TYPE_ACTOR 2

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
    SimEntId id;
    i32 type;
    Transform t;
    Vec3 velocity;
};

struct SimEntityDef
{
    i32 serial;
    i32 isLocal;
    f32 pos[3];
    f32 scale[3];
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

    // Command buffers
    DoubleByteBuffer commands;
};


////////////////////////////////////////////
// 
////////////////////////////////////////////
