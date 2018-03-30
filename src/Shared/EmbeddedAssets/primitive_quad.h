#pragma once

#include "../shared.h"

f32 g_prim_quadVerts[] =
{
	-0.5, -0.5,  0,
	 0.5, -0.5,  0,
	 0.5,  0.5,  0,

	-0.5, -0.5,  0,
	 0.5,  0.5,  0,
	-0.5,  0.5,  0
};

f32 g_prim_quadUVs[] =
{
	0, 0,
	1, 0,
	1, 1,

	0, 0,
	1, 1,
	0, 1
};

f32 g_prim_quadNormals[] =
{
	-1, -1,  0,
	 1, -1,  0,
	 1,  1,  0,

	-1, -1,  0,
	 1,  1,  0,
	-1,  1,  0
};

Mesh g_meshPrimitive_quad = {
    0,
    6,
    g_prim_quadVerts,
    g_prim_quadUVs

};

