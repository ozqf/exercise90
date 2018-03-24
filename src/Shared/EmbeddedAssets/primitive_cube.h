#pragma once

#include "../shared.h"

/*
6 sides, 2 triangles per side = 12 tris
12 * 3 points, each with pos (vec3), normal (vec3), colour (vec3) and uv (vec2)

	v6	v7

	v4	v5
  ________
 /		/ |
/______/  |
|v2	v3 |  |
|	   | /
|______|/
v0		v1
*/
/*
Cube = 6 quads
6 verts per quad =  36 verts
3 floats per vert = 108 floats
*/

f32 g_prim_cubeVerts[] =
{
    // face 0 (front)
    -1, -1,  1,
     1, -1,  1,
     1,  1,  1,
    -1, -1,  1,
     1,  1,  1,
    -1,  1,  1,
    // face 1 (right side)
     1, -1,  1,
     1, -1, -1,
     1,  1, -1,
     1, -1,  1,
     1,  1, -1,
     1,  1,  1,
    // face 2 (back)
     1, -1, -1,
    -1, -1, -1,
    -1,  1, -1,
     1, -1, -1,
    -1,  1, -1,
     1,  1, -1,
    // face 3 (left side)
    -1, -1, -1,
    -1, -1,  1,
    -1,  1,  1,
    -1, -1, -1,
    -1,  1,  1,
    -1,  1, -1,
    // face 4 (top)
    -1,  1,  1,
     1,  1,  1,
     1,  1, -1,
    -1,  1,  1,
     1,  1, -1,
    -1,  1, -1,
    // face 5 (bottom)
    -1, -1, -1,
     1, -1, -1,
     1, -1,  1,
    -1, -1, -1,
     1, -1,  1,
    -1, -1,  1
};

f32 g_prim_cubeUVs[] =
{
    // face 0
    0, 0,
    1, 0,
    1, 1,
    0, 0,
    1, 1,
    0, 1,
    // face 1
    0, 0,
    1, 0,
    1, 1,
    0, 0,
    1, 1,
    0, 1,
    // face 2
    0, 0,
    1, 0,
    1, 1,
    0, 0,
    1, 1,
    0, 1,
    // face 3
    0, 0,
    1, 0,
    1, 1,
    0, 0,
    1, 1,
    0, 1,
    // face 4
    0, 0,
    1, 0,
    1, 1,
    0, 0,
    1, 1,
    0, 1,
    // face 5
    0, 0,
    1, 0,
    1, 1,
    0, 0,
    1, 1,
    0, 1
};

f32 g_prim_cubeNormals[] = 
{
        // face 0 (front)
     0,  0,  1,
     0,  0,  1,
     0,  0,  1,
     0,  0,  1,
     0,  0,  1,
     0,  0,  1,
    // face 1 (right side)
     1,  0,  0,
     1,  0,  0,
     1,  0,  0,
     1,  0,  0,
     1,  0,  0,
     1,  0,  0,
    // face 2 (back)
     0,  0, -1,
     0,  0, -1,
     0,  0, -1,
     0,  0, -1,
     0,  0, -1,
     0,  0, -1,
    // face 3 (left side)
    -1,  0,  0,
    -1,  0,  0,
    -1,  0,  0,
    -1,  0,  0,
    -1,  0,  0,
    -1,  0,  0,
    // face 4 (top)
     0,  1,  0,
     0,  1,  0,
     0,  1,  0,
     0,  1,  0,
     0,  1,  0,
     0,  1,  0,
    // face 5 (bottom)
     0, -1,  0,
     0, -1,  0,
     0, -1,  0,
     0, -1,  0,
     0, -1,  0,
     0, -1,  0
};

Mesh g_meshCube = {
    0,
    36,
    g_prim_cubeVerts,
    g_prim_cubeUVs,
    g_prim_cubeNormals
};
