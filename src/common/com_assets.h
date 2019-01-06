/*
Very basic meshes/textures embedded in source
> Quad
> Cube

*/
#pragma once

#include "com_module.h"

// com_internal f32 g_prim_quadVerts[];
// com_internal f32 g_prim_quadUVs[];
// com_internal f32 g_prim_quadNormals[];
// com_internal Mesh g_meshPrimitive_quad;
com_internal void SharedAssets_Init();

com_internal MeshData* COM_GetCubeMesh();
