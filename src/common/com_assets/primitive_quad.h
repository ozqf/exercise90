#pragma once

#include "com_module.h"

internal f32 g_prim_quadVerts[] =
{
	-0.5, -0.5,  0,
	 0.5, -0.5,  0,
	 0.5,  0.5,  0,

	-0.5, -0.5,  0,
	 0.5,  0.5,  0,
	-0.5,  0.5,  0
};

internal f32 g_prim_quadUVs[] =
{
	0, 0,
	1, 0,
	1, 1,

	0, 0,
	1, 1,
	0, 1
};

internal f32 g_prim_quadNormals[] =
{
	-1, -1,  0,
	 1, -1,  0,
	 1,  1,  0,

	-1, -1,  0,
	 1,  1,  0,
	-1,  1,  0
};

internal Mesh g_meshPrimitive_quad = {
    0,
	{
		'Q', 'u', 'a', 'd', '\0',
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0', '\0', '\0', '\0', 
        '\0', '\0'
	},
    6,
    g_prim_quadVerts,
    g_prim_quadUVs

};

