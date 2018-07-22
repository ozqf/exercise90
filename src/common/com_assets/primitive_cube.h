#pragma once

#include "com_module.h"

////////////////////////////////////////////////////////////////////////////////////////
// Solid cube
////////////////////////////////////////////////////////////////////////////////////////
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

#define P_SIZE 0.5

internal f32 g_prim_cubeVerts[] =
{
    // face 0 (front)
    -0.5, -0.5,  0.5,
     0.5, -0.5,  0.5,
     0.5,  0.5,  0.5,
    -0.5, -0.5,  0.5,
     0.5,  0.5,  0.5,
    -0.5,  0.5,  0.5,
    // face 0.5 (right side)
     0.5, -0.5,  0.5,
     0.5, -0.5, -0.5,
     0.5,  0.5, -0.5,
     0.5, -0.5,  0.5,
     0.5,  0.5, -0.5,
     0.5,  0.5,  0.5,
    // face 2 (back)
     0.5, -0.5, -0.5,
    -0.5, -0.5, -0.5,
    -0.5,  0.5, -0.5,
     0.5, -0.5, -0.5,
    -0.5,  0.5, -0.5,
     0.5,  0.5, -0.5,
    // face 3 (left side)
    -0.5, -0.5, -0.5,
    -0.5, -0.5,  0.5,
    -0.5,  0.5,  0.5,
    -0.5, -0.5, -0.5,
    -0.5,  0.5,  0.5,
    -0.5,  0.5, -0.5,
    // face 4 (top)
    -0.5,  0.5,  0.5,
     0.5,  0.5,  0.5,
     0.5,  0.5, -0.5,
    -0.5,  0.5,  0.5,
     0.5,  0.5, -0.5,
    -0.5,  0.5, -0.5,
    // face 5 (bottom)
    -0.5, -0.5, -0.5,
     0.5, -0.5, -0.5,
     0.5, -0.5,  0.5,
    -0.5, -0.5, -0.5,
     0.5, -0.5,  0.5,
    -0.5, -0.5,  0.5
};

#undef P_SIZE

internal f32 g_prim_cubeUVs[] =
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

internal f32 g_prim_cubeNormals[] = 
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

internal Mesh g_meshCube = {
    0,
    {   'C', 'u', 'b', 'e', '\0',
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0' 
    },
    36,
    g_prim_cubeVerts,
    g_prim_cubeUVs,
    g_prim_cubeNormals
    
};


////////////////////////////////////////////////////////////////////////////////////////
// Inverse (inside-out) cube
////////////////////////////////////////////////////////////////////////////////////////


internal f32 g_prim_inverseCubeVerts[] =
{
    // face 2 (back)
     0.5, -0.5, 0.5,
    -0.5, -0.5, 0.5,
    -0.5,  0.5, 0.5,
     0.5, -0.5, 0.5,
    -0.5,  0.5, 0.5,
     0.5,  0.5, 0.5,
    // face 3 (left side)
    0.5, -0.5, -0.5,
    0.5, -0.5,  0.5,
    0.5,  0.5,  0.5,
    0.5, -0.5, -0.5,
    0.5,  0.5,  0.5,
    0.5,  0.5, -0.5,
    // face 0 (front)
    -0.5, -0.5,  -0.5,
     0.5, -0.5,  -0.5,
     0.5,  0.5,  -0.5,
    -0.5, -0.5,  -0.5,
     0.5,  0.5,  -0.5,
    -0.5,  0.5,  -0.5,
    
    
    // face 0.5 (right side)
    -0.5, -0.5,  0.5,
    -0.5, -0.5, -0.5,
    -0.5,  0.5, -0.5,
    -0.5, -0.5,  0.5,
    -0.5,  0.5, -0.5,
    -0.5,  0.5,  0.5,
    // face 5 (bottom)
    -0.5, 0.5, -0.5,
     0.5, 0.5, -0.5,
     0.5, 0.5,  0.5,
    -0.5, 0.5, -0.5,
     0.5, 0.5,  0.5,
    -0.5, 0.5,  0.5,
    // face 4 (top)
    -0.5, -0.5,  0.5,
     0.5, -0.5,  0.5,
     0.5, -0.5, -0.5,
    -0.5, -0.5,  0.5,
     0.5, -0.5, -0.5,
    -0.5, -0.5, -0.5
};

internal f32 g_prim_inverseCubeUVs[] =
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

internal f32 g_prim_inverseCubeNormals[] = 
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

internal Mesh g_meshInverseCube = {
    0,
    {   'I', 'n', 'v', 'e', 'r',
        's', 'e', '_', 'C', 'u', 
        'b', 'e', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0' 
    },
    36,
    g_prim_inverseCubeVerts,
    g_prim_inverseCubeUVs,
    g_prim_inverseCubeNormals
};

