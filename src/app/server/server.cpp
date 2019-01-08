#pragma once

#include <stdlib.h>
#include "../../common/com_module.h"
#include "server.h"
#include "../../sim/sim.h"

#define SV_MAX_MALLOCS 1024
internal MallocItem g_mallocItems[SV_MAX_MALLOCS];
internal MallocList g_mallocs;
internal UserList g_users;
internal i32 g_isRunning = 0;
internal SimScene g_sim;
i32 SV_IsRunning() { return g_isRunning; }

void SV_LoadTestScene()
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

internal void SV_ListAllocs()
{
    printf("-- SV ALLOCS --\n");
    for (i32 i = 0; i < g_mallocs.max; ++i)
    {
        MallocItem* item = &g_mallocs.items[i];
        if (item->ptr == NULL) { continue; }
        printf("%s: %d bytes\n", item->label, item->capacity);
    }
}

void SV_Init()
{
    printf("SV Init scene\n");

    g_mallocs = COM_InitMallocList(g_mallocItems, SV_MAX_MALLOCS);

    i32 size = MegaBytes(1);
    ByteBuffer a = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, size, "Sim Buf A"), size);
    ByteBuffer b = Buf_FromMalloc(
        COM_Malloc(&g_mallocs, size, "Sim Buf B"), size);

    i32 maxEnts = 2048;
    size = Sim_CalcEntityArrayBytes(maxEnts);
    SimEntity* mem = (SimEntity*)COM_Malloc(&g_mallocs, size, "Sim Ents");
    Sim_InitScene(&g_sim, a, b, mem, maxEnts);
    SV_LoadTestScene();
    SV_ListAllocs();
}

void SV_Shutdown()
{
    for (i32 i = 0; i < g_mallocs.max; ++i)
    {
        if (g_mallocs.items[i].ptr != NULL)
        {
            free(g_mallocs.items[i].ptr);
        }
    }
}

void SV_Tick(f32 deltaTime)
{
    Sim_Tick(&g_sim, deltaTime);

    ByteBuffer* buf = App_GetLocalClientPacket();
}

void SV_PopulateRenderScene(RenderScene* scene, i32 maxObjects, i32 texIndex, f32 interpolateTime)
{

    for (i32 j = 0; j < g_sim.maxEnts; ++j)
    {
        SimEntity* ent = &g_sim.ents[j];
        if (ent->status != SIM_ENT_STATUS_IN_USE) { continue; }

        RendObj obj = {};
        MeshData* cube = COM_GetCubeMesh();
        RendObj_SetAsMesh(
            &obj, *cube, 1, 0, 0, texIndex);
        RScene_AddRenderItem(scene, &ent->t, &obj);
    }
}
