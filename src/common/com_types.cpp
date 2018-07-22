#pragma once

#include "com_module.h"

inline u32 COM_CalcSizeOfMesh(MeshData* m)
{
    u32 total = 0;
    // verts
    total += (sizeof(f32) * 3) * m->numVerts;
    // uvs
    total += (sizeof(f32) * 2) * m->numVerts;
    // normals
    total += (sizeof(f32) * 3) * m->numVerts;
    return total;
}
