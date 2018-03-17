#pragma once

#include "../Shared/shared.h"

#define RENDOBJ_TYPE_NONE 0
#define RENDOBJ_TYPE_PRIMITIVE 1
#define RENDOBJ_TYPE_BILLBOARD 2
#define RENDOBJ_TYPE_MESH 3
#define RENDOBJ_TYPE_LINE 4
#define RENDOBJ_TYPE_ASCI_CHAR 5

struct Texture2DHeader
{
	i32 id;
	char name[32];
	i32 width;
	i32 height;
	u32 *ptrMemory;
};

struct RendObj_Primitive
{
    i32 primitiveType;
    f32 red;
    f32 green;
    f32 blue;
    f32 alpha;
};

struct RendObj_Billboard
{
    i32 textureIndex;
    f32 r;
    f32 g;
    f32 b;
    f32 a;
};

struct RendObj_Line
{
    Vec3 a;
    Vec3 b;
    Vec3 colourA;
    Vec3 colourB;
};

struct RendObj_AsciChar
{
    u8 asciChar;
};

struct RendObj_ColouredMesh
{
    Mesh* mesh;
    f32 r, g, b;
    i32 textureIndex;
};

union RendObj_Union
{
    RendObj_Primitive primitive;
    RendObj_Billboard billboard;
    RendObj_ColouredMesh mesh;
    RendObj_AsciChar asciChar;
};

// Individual render item
struct RendObj
{
    u32 type;
    i32 ID;
    Transform transform;
    RendObj_Union obj;
};

// Scene container for render call each frame
struct RenderScene
{
    RendObj* rendObjects;
    u32 numObjects;
    u32 maxObjects;
    Transform cameraTransform;

    RendObj* uiObjects;
    u32 numUIObjects;
    u32 maxUIObjects;
};

#define REND_PRIMITIVE_TYPE_RAINBOW_QUAD 0
#define REND_PRIMITIVE_TYPE_SINGLE_COLOUR_QUAD 1

void RendObj_SetAsMesh(RendObj* obj, Mesh* mesh, f32 red, f32 green, f32 blue, i32 textureIndex)
{
    obj->type = RENDOBJ_TYPE_MESH;

    RendObj_ColouredMesh* rend = &obj->obj.mesh;
    rend->mesh = mesh;
    rend->r = red;
    rend->g = green;
    rend->b = blue;
    rend->textureIndex = textureIndex;   
}

void RendObj_SetAsRainbowQuad(RendObj* obj)
{
    obj->type = RENDOBJ_TYPE_PRIMITIVE;
    
    RendObj_Primitive* prim = &obj->obj.primitive;
    prim->primitiveType = REND_PRIMITIVE_TYPE_RAINBOW_QUAD;
}

void RendObj_SetAsColouredQuad(RendObj* obj, f32 red, f32 green, f32 blue)
{
    obj->type = RENDOBJ_TYPE_PRIMITIVE;

    RendObj_Primitive* prim = &obj->obj.primitive;
    prim->primitiveType = REND_PRIMITIVE_TYPE_SINGLE_COLOUR_QUAD;
    prim->red = red;
    prim->green = green;
    prim->blue = blue;
    prim->alpha = 1;
}

void RendObj_SetAsBillboard(RendObj* obj, f32 r, f32 g, f32 b, i32 textureIndex)
{
    obj->type = RENDOBJ_TYPE_BILLBOARD;
    RendObj_Billboard* rend = &obj->obj.billboard;
    rend->r = r;
    rend->g = g;
    rend->b = b;
    rend->a = 1;
    rend->textureIndex = textureIndex;
}

void RendObj_SetAsAsciChar(RendObj* obj, u8 asciCharacter)
{
    obj->type = RENDOBJ_TYPE_ASCI_CHAR;
    RendObj_AsciChar* c = &obj->obj.asciChar;
    c->asciChar = asciCharacter;
}
