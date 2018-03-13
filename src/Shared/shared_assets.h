#pragma once

#include "shared.h"

Mesh g_meshPrimitive_quad;
// 2 triangles, 3 verts per tri, 3 floats per vert
f32 g_meshPrimitiveQuad_Verts[(2 * 3 * 3)];

void ZGL_SetTriangleInArray(
	f32* array,
	f32 aX, f32 aY, f32 aZ,
	f32 bX, f32 bY, f32 bZ,
	f32 cX, f32 cY, f32 cZ
	)

{
    // u32 triangleNumber;
	// triangleNumber = 0;
	// array[triangleNumber + 0] = aX;
	// array[triangleNumber + 1] = aY;
	// array[triangleNumber + 2] = aZ;
	// triangleNumber = 1 * 3;
	// array[triangleNumber + 0] = bX;
	// array[triangleNumber + 1] = bY;
	// array[triangleNumber + 2] = bZ;
	// triangleNumber = 2 * 3;
	// array[triangleNumber + 0] = cX;
	// array[triangleNumber + 1] = cY;
	// array[triangleNumber + 2] = cZ;
	array[0] = aX;
	array[1] = aY;
	array[2] = aZ;
	
	array[3] = bX;
	array[4] = bY;
	array[5] = bZ;
	
	array[6] = cX;
	array[7] = cY;
	array[8] = cZ;
}

void SharedAssets_Init()
{
    g_meshPrimitive_quad = {};
    g_meshPrimitive_quad.verts = g_meshPrimitiveQuad_Verts;
    g_meshPrimitive_quad.numVerts = 6;

	ZGL_SetTriangleInArray(g_meshPrimitiveQuad_Verts,
		-1, -1, 0,
		1, -1, 0,
		1, 1, 0);
	
	ZGL_SetTriangleInArray(g_meshPrimitiveQuad_Verts + 9,
		-1, -1, 0,
		1, 1, 0,
		-1, 1, 0);
}