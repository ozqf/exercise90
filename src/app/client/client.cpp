#pragma once

#include <stdlib.h>
#include "../../common/com_module.h"
#include "client.h"
#include "../../sim/sim.h"

internal i32 g_isRunning = 0;
internal SimScene g_sim;
internal i32 g_ticks = 0;
i32 CL_IsRunning() { return g_isRunning; }

#define CL_MAX_ALLOCATIONS 256
internal void* g_allocations[CL_MAX_ALLOCATIONS];
internal i32 g_bytesAllocated = 0;
internal i32 g_numAllocations = 0;

internal NetStream g_reliableStream;
internal NetStream g_unreliableStream;

internal void* CL_Malloc(i32 numBytes)
{
    Assert(g_numAllocations < CL_MAX_ALLOCATIONS)
    i32 index = g_numAllocations++;
    g_allocations[index] = malloc(numBytes);
    g_bytesAllocated += numBytes;
    return g_allocations[index];
}

void CL_LoadTestScene()
{
    SimEntityDef def = {};
    #if 1
    for (i32 i = 0; i < 8; ++i)
    {
        f32 randX = (COM_STDRandf32() * 2) - 1;
        f32 randZ = (COM_STDRandf32() * 2) - 1;
        f32 x = 2 * randX;
        f32 y = 1;
        f32 z = 2 * randZ;
        def.isLocal = 1;
        def.pos[0] = x;
        def.pos[1] = y;
        def.pos[2] = z;
        def.velocity[0] = x;
        def.velocity[2] = z;
        Sim_AddEntity(&g_sim, &def);
    }
    #endif
    #if 1
    def = {};
    def.isLocal = 1;
    def.pos[1] = 0;
    def.scale[0] = 12;
    def.scale[1] = 1;
    def.scale[2] = 12;
    Sim_AddEntity(&g_sim, &def);

    g_sim.boundaryMin = { -6, -6, -6 };
    g_sim.boundaryMax = { 6, 6, 6 };

    #endif
}

void CL_SetLocalUser(UserIds ids)
{
    printf("CL Set local user public %d private %d\n",
        ids.publicId, ids.privateId
    );
}

void CL_Init()
{
    printf("CL Init scene\n");
    i32 cmdBufferSize = MegaBytes(1);
    ByteBuffer a = Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize);
    ByteBuffer b = Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize);

    i32 maxEnts = 2048;
    i32 numEntityBytes = Sim_CalcEntityArrayBytes(maxEnts);
    SimEntity* mem = (SimEntity*)CL_Malloc(numEntityBytes);
    Sim_InitScene(&g_sim, a, b, mem, maxEnts);
    CL_LoadTestScene();

    COM_InitStream(&g_reliableStream,
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize),
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize)
    );
    COM_InitStream(&g_unreliableStream,
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize),
        Buf_FromMalloc(CL_Malloc(cmdBufferSize), cmdBufferSize)
    );
    printf("CL init completed with %d allocations (%dKB)\n ",
        g_numAllocations, (u32)BytesAsKB(g_bytesAllocated));
}

void CL_Shutdown()
{
    // TODO: Free memory (:
}

void CL_ReadReliableCommands(NetStream* stream)
{
    ByteBuffer* b = &stream->inputBuffer;
    u8* read = b->ptrStart;
    u8* end = b->ptrEnd;
    while (read < end)
    {
        Command* header = (Command*)read;
        Assert(header->sentinel == CMD_SENTINEL)
        Assert(header->size > 0)
        read += header->size;
        switch (header->type)
        {
            case CMD_TYPE_IMPULSE:
            {
                CmdImpulse* cmd = (CmdImpulse*)header;
            } break;
            default:
            {
                printf("CL Unknown command type %d\n", header->type);
                ILLEGAL_CODE_PATH
            } break;
        }
    }
}

void CL_Tick(f32 deltaTime)
{
    Sim_Tick(&g_sim, deltaTime);
    g_ticks++;
}

void CL_PopulateRenderScene(RenderScene* scene, i32 maxObjects, i32 texIndex, f32 interpolateTime)
{

    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }

        RendObj obj = {};
        MeshData* cube = COM_GetCubeMesh();
        RendObj_SetAsMesh(
            &obj, *cube, 0.3f, 0.3f, 1, texIndex);

        Transform t = ent->t;
        RendObj_InterpolatePosition(
                &t.pos,
                &ent->previousPos,
                &ent->t.pos,
                interpolateTime);
        RScene_AddRenderItem(scene, &t, &obj);
    }
}
