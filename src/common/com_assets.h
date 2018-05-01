/*
Very basic meshes/textures embedded in source
> Quad
> Cube

*/
#pragma once

#include "com_module.h"

extern f32 g_prim_quadVerts[];
extern f32 g_prim_quadUVs[];
extern f32 g_prim_quadNormals[];
extern Mesh g_meshPrimitive_quad;
void SharedAssets_Init();
