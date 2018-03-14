#pragma once

#include "../Shared/shared.h"

#define RENDOBJ_TYPE_NONE 0
#define RENDOBJ_TYPE_PRIMITIVE 1
#define RENDOBJ_TYPE_BILLBOARD 2
#define RENDOBJ_TYPE_MESH 3
#define RENDOBJ_TYPE_LINE 4

struct RendObj_Primitive
{
    i32 primitiveType;
    f32 red;
    f32 green;
    f32 blue;
};

struct RendObj_Billboard
{

};

struct RendObj_Line
{
    Vec3 a;
    Vec3 b;
    Vec3 colourA;
    Vec3 colourB;
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
    RendObj_ColouredMesh mesh;
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
};

void RendObj_SetAsColouredQuad(RendObj* obj, f32 red, f32 green, f32 blue)
{
    obj->type = RENDOBJ_TYPE_PRIMITIVE;

    RendObj_Primitive* prim = &obj->obj.primitive;
    prim->primitiveType = REND_PRIMITIVE_TYPE_SINGLE_COLOUR_QUAD;
    prim->red = red;
    prim->green = green;
    prim->blue = blue;
}
