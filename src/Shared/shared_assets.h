/*
Very basic meshes/textures embedded in source
> Quad
> Cube

*/
#pragma once

#include "shared.h"

#include "EmbeddedAssets/primitive_quad.h"
#include "EmbeddedAssets/primitive_cube.h"

Mesh g_meshPrimitive_quad2;
// 2 triangles, 3 verts per tri, 3 floats per vert
f32 g_meshPrimitiveQuad_Verts[(2 * 3 * 3)];
f32 g_meshPrimitiveQuad_uvs[(2 * 3 * 2)];

f32 g_prim_quadVerts2[] =
{
	-1, -1, 0,
	1, -1, 0,
	1, 1, 0,
	-1, -1, 0,
	1, 1, 0,
	-1, 1, 0
};

f32 g_prim_quadUVs2[] =
{
	0, 0,
	1, 0,
	1, 1,
	0, 0,
	1, 1,
	0, 1
};

void ZGL_SetTriangleInArray(
	f32* array,
	f32 aX, f32 aY, f32 aZ,
	f32 bX, f32 bY, f32 bZ,
	f32 cX, f32 cY, f32 cZ
	)

{
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

void ZGL_SetTriangleUVsInArray(
	f32* array,
	f32 vertATexX, f32 vertATexY,
	f32 vertBTexX, f32 vertBTexY,
	f32 vertCTexX, f32 vertCTexY
	)
{
	array[0] = vertATexX;
	array[1] = vertATexY;
	array[2] = vertBTexX;
	array[3] = vertBTexY;
	array[4] = vertCTexX;
	array[5] = vertCTexY;
}

void SharedAssets_Init()
{
    g_meshPrimitive_quad = {};
    // g_meshPrimitive_quad.verts = g_meshPrimitiveQuad_Verts;
	// g_meshPrimitive_quad.uvs = g_meshPrimitiveQuad_uvs;
	g_meshPrimitive_quad.verts = g_prim_quadVerts;
	g_meshPrimitive_quad.uvs = g_prim_quadUVs;
    g_meshPrimitive_quad.numVerts = 6;

	ZGL_SetTriangleInArray(g_meshPrimitiveQuad_Verts,
		-1, -1, 0,
		1, -1, 0,
		1, 1, 0);
	
	ZGL_SetTriangleInArray(g_meshPrimitiveQuad_Verts + 9,
		-1, -1, 0,
		1, 1, 0,
		-1, 1, 0);
	
	ZGL_SetTriangleUVsInArray(g_meshPrimitiveQuad_uvs, 
		0, 0,
		1, 0,
		1, 1);

	ZGL_SetTriangleUVsInArray(g_meshPrimitiveQuad_uvs + 6, 
		0, 0,
		1, 1,
		0, 1);
}
