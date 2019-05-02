/*
Very basic meshes/textures embedded in source
> Quad
> Cube

*/
#pragma once

#include "common.h"

com_internal MeshData* g_meshes[COM_MAX_MESHES];
com_internal i32 g_bAssetsInitialised = NO;

// .h but should be invisible to other modules
#include "com_assets/primitive_quad.h"
#include "com_assets/primitive_cube.h"
#include "com_assets/primitive_octahedron.h"
#include "com_assets/primitive_spike.h"
#include "com_assets/embedded_textures.h"



com_internal void COM_InitEmbeddedAssets()
{
    if (g_bAssetsInitialised == YES) { return; }
    g_bAssetsInitialised = YES;

    g_meshes[COM_MESH_CUBE_INDEX] = &g_meshCube;
    g_meshes[COM_MESH_INVERSE_CUBE_INDEX] = &g_meshInverseCube;
    g_meshes[COM_MESH_SPIKE_INDEX] = &g_meshSpike;
    g_meshes[COM_MESH_OCTAHEDRON_INDEX] = &g_meshOctahedron;
}

com_internal MeshData* COM_GetCubeMesh()
{
    return &g_meshCube;
}

com_internal MeshData* COM_GetInverseCubeMesh()
{
    return &g_meshInverseCube;
}

com_internal MeshData* COM_GetSpikeMesh()
{
    return &g_meshSpike;
}

com_internal MeshData* COM_GetOctahedronMesh()
{
    return &g_meshOctahedron;
}

com_internal MeshData* COM_GetEmbeddedMesh(i32 index)
{
    COM_ASSERT(g_bAssetsInitialised, "Common assets not initialised!");
    if (index < 0 || index >= COM_MESH_OUT_OF_BOUNDS_INDEX)
    { index = 0; }
    return g_meshes[index];
}

com_internal u8* Embed_GetCharset128x128BW()
{
    return charset128x128_BW;
}
