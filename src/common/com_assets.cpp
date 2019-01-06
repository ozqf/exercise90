/*
Very basic meshes/textures embedded in source
> Quad
> Cube

*/
#pragma once

#include "com_module.h"

// .h but should be invisible to other modules
#include "com_assets/primitive_quad.h"
#include "com_assets/primitive_cube.h"
#include "com_assets/primitive_octahedron.h"
#include "com_assets/primitive_spike.h"

com_internal inline void SharedAssets_Init() {}

com_internal inline MeshData* COM_GetCubeMesh()
{
    return &g_meshCube;
}
