#pragma once

#include "../shared.h"

f32 g_prim_octahedronVerts[] =
{
    // TOP
    // face 0
    -1,  0,  1,
     1,  0,  1,
     0,  1,  0,
    // face 1
    1, 0, 1,
    1, 0, -1,
    0, 1, 0,
    // face 2
    1, 0, -1,
    -1, 0, -1,
    0, 1, 0,
    // face 3
    -1, 0, -1,
    -1, 0, 1,
    0, 1, 0,

    // BOTTOM
    // face 4
    -1,  0,  1,
     0,  -1,  0,
     1,  0,  1,
    // face 5
    1, 0, 1,
    0, -1, 0,
    1, 0, -1,
    // face 6
    1, 0, -1,
    0, -1, 0,
    -1, 0, -1,
    // face 7
    -1, 0, -1,
    0, -1, 0,
    -1, 0, 1
};

f32 g_prim_octahedronUVs[] =
{
    // face 0
    0, 0,
    1, 0,
    0.5, 1,
    // face 1
    0, 0,
    1, 0,
    0.5, 1,
    // face 2
    0, 0,
    1, 0,
    0.5, 1,
    // face 3
    0, 0,
    1, 0,
    0.5, 1,
    // face 4
    0, 1,
    0.5, 0,
    1, 1,
    // face 5
    0, 1,
    0.5, 0,
    1, 1,
    // face 6
    0, 1,
    0.5, 0,
    1, 1,
    // face 7
    0, 1,
    0.5, 0,
    1, 1,
};

f32 g_prim_octahedronNormals[] =
{
    // TOP
    // face 0
    -1,  0,  1,
     1,  0,  1,
     0,  1,  0,
    // face 1
    1, 0, 1,
    1, 0, -1,
    0, 1, 0,
    // face 2
    1, 0, -1,
    -1, 0, -1,
    0, 1, 0,
    // face 3
    -1, 0, -1,
    -1, 0, 1,
    0, 1, 0

    // BOTTOM
    // face 4
    -1,  0,  1,
     0,  -1,  0,
     1,  0,  1,
    // face 5
    1, 0, 1,
    0, -1, 0,
    1, 0, -1,
    // face 6
    1, 0, -1,
    0, -1, 0,
    -1, 0, -1,
    // face 7
    -1, 0, -1,
    0, -1, 0
    -1, 0, 1,
};


Mesh g_meshOctahedron = {
    0,
    24,
    g_prim_octahedronVerts,
    g_prim_octahedronUVs,
    g_prim_octahedronNormals
};
