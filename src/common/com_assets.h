/*
Very basic meshes/textures embedded in source
> Quad
> Cube

*/
#pragma once

#include "common.h"


#define COM_MESH_CUBE_INDEX 0
#define COM_MESH_INVERSE_CUBE_INDEX 1
#define COM_MESH_SPIKE_INDEX 2
#define COM_MESH_OCTAHEDRON_INDEX 3

#define COM_MESH_OUT_OF_BOUNDS_INDEX 4

#define COM_MAX_MESHES 256

com_internal void COM_InitEmbeddedAssets();
com_internal MeshData* COM_GetEmbeddedMesh(i32 index);

com_internal MeshData* COM_GetCubeMesh();
com_internal MeshData* COM_GetInverseCubeMesh();
com_internal u8* Embed_GetCharset128x128BW();
