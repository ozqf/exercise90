/*
Very basic meshes/textures embedded in source
> Quad
> Cube

*/

#include "common.h"

// .h but should be invisible to other modules
#include "com_assets/primitive_quad.h"
#include "com_assets/primitive_cube.h"
#include "com_assets/primitive_octahedron.h"
#include "com_assets/primitive_spike.h"
#include "com_assets/embedded_textures.h"

com_internal void SharedAssets_Init() {}

com_internal MeshData* COM_GetCubeMesh()
{
    return &g_meshCube;
}
com_internal MeshData* COM_GetInverseCubeMesh()
{
    return &g_meshInverseCube;
}

com_internal u8* Embed_GetCharset128x128BW()
{
    return charset128x128_BW;
}
