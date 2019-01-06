#pragma once

#include "../common/com_module.h"

#define SIM_ENT_BLOCK_SIZE 128
#define SIM_ENT_MAX_BLOCKS 128

#define SIM_ENT_STATUS_FREE 0
#define SIM_ENT_STATUS_RESERVED 1
#define SIM_ENT_STATUS_IN_USE 2

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
    i32 sequence;
};

struct SimEntity
{
    i32 status;
    SimEntId id;
    Transform t;
    Vec3 velocity;
};

struct SimEntBlock
{
    i32 index;
    SimEntity* ents;
    i32 capacity;
};

struct SimScene
{
    i32 blockSize;
    i32 numBlocks;
    i32 maxBlocks;

    // sequential, unrelated to blocks
    i32 entSequence;
    i32 cmdSequence;
    u32 tick;

    // Command buffers
    DoubleByteBuffer commands;

    SimEntBlock blocks[SIM_ENT_MAX_BLOCKS];
};


////////////////////////////////////////////
// 
////////////////////////////////////////////
