#pragma once

#include "shared.h"

///////////////////////////////////////////////////////////////////
// Build a render scene
///////////////////////////////////////////////////////////////////

inline void RScene_AddRenderItem(RenderScene* scene, Transform* t, RendObj* rendObj)
{
    Assert(scene->numObjects < scene->maxObjects);
    RenderListItem* item = &scene->sceneItems[scene->numObjects];
    scene->numObjects++;
    item->transform = *t;
    item->obj = *rendObj;
}


///////////////////////////////////////////////////////////////////
// Init Render Objects
///////////////////////////////////////////////////////////////////

void RendObj_SetAsMesh(RendObj* obj, Mesh* mesh, f32 red, f32 green, f32 blue, i32 textureIndex)
{
    obj->type = RENDOBJ_TYPE_MESH;

    RendObj_ColouredMesh* rend = &obj->data.mesh;
    rend->mesh = mesh;
    rend->r = red;
    rend->g = green;
    rend->b = blue;
    rend->textureIndex = textureIndex;   
}

void RendObj_SetAsRainbowQuad(RendObj* obj)
{
    obj->type = RENDOBJ_TYPE_PRIMITIVE;
    
    RendObj_Primitive* prim = &obj->data.primitive;
    prim->primitiveType = REND_PRIMITIVE_TYPE_RAINBOW_QUAD;
}

void RendObj_SetAsAABB(RendObj* obj, f32 sizeX, f32 sizeY, f32 sizeZ,
    f32 red, f32 green, f32 blue)
{
    obj->type = RENDOBJ_TYPE_PRIMITIVE;
    
    RendObj_Primitive* prim = &obj->data.primitive;
    prim->primitiveType = REND_PRIMITIVE_TYPE_AABB;
    prim->sizeX = sizeX;
    prim->sizeY = sizeY;
    prim->sizeZ = sizeZ;
    prim->red = red;
    prim->green = green;
    prim->blue = blue;
}

void RendObj_SetAsColouredQuad(RendObj* obj, f32 red, f32 green, f32 blue)
{
    obj->type = RENDOBJ_TYPE_PRIMITIVE;

    RendObj_Primitive* prim = &obj->data.primitive;
    prim->primitiveType = REND_PRIMITIVE_TYPE_SINGLE_COLOUR_QUAD;
    prim->red = red;
    prim->green = green;
    prim->blue = blue;
    prim->alpha = 1;
}

void RendObj_SetAsSprite(RendObj* obj,
    i32 mode, i32 textureIndex,
    f32 width, f32 height)
{
    obj->type = RENDOBJ_TYPE_SPRITE;
    RendObj_Sprite* rend = &obj->data.sprite;
    rend->textureIndex = textureIndex;
    rend->mode = mode;

    rend->width = width;
    rend->height = height;

    rend->uvLeft = 0;
    rend->uvRight = 1;
    rend->uvBottom = 0;
    rend->uvTop = 1;
    
    rend->r = 1;
    rend->g = 1;
    rend->b = 1;
    rend->a = 1;
}

void RendObj_SetAsLine(RendObj* obj,
    f32 x0, f32 y0, f32 z0,
    f32 x1, f32 y1, f32 z1,
    f32 r0, f32 g0, f32 b0,
    f32 r1, f32 g1, f32 b1
    )
{
    obj->type = RENDOBJ_TYPE_LINE;
    RendObj_Line* rend = &obj->data.line;
    rend->a.x = x0; rend->a.y = y0; rend->a.z = z0;
    rend->b.x = x1; rend->b.y = y1; rend->b.z = z1;

    rend->colourA.x = r0; rend->colourA.y = g0; rend->colourA.z = b0;
    rend->colourB.x = r1; rend->colourB.y = g1; rend->colourB.z = b1;
}

void RendObj_SetSpriteUVs(RendObj_Sprite* sprite,
    f32 uvLeft, f32 uvRight, f32 uvBottom, f32 uvTop)
{
    sprite->uvLeft = uvLeft;
    sprite->uvRight = uvRight;
    sprite->uvBottom = uvBottom;
    sprite->uvTop = uvTop;
}

void RendObj_SetAsBillboard(RendObj* obj, f32 r, f32 g, f32 b, i32 textureIndex)
{
    obj->type = RENDOBJ_TYPE_BILLBOARD;
    RendObj_Billboard* rend = &obj->data.billboard;
    rend->r = r;
    rend->g = g;
    rend->b = b;
    rend->a = 1;
    rend->uvLeft = 0;
    rend->uvRight = 1;
    rend->uvBottom = 0;
    rend->uvTop = 1;
    rend->textureIndex = textureIndex;
}

void RendObj_SetAsAsciChar(RendObj* obj, u8 asciCharacter)
{
    obj->type = RENDOBJ_TYPE_ASCI_CHAR;
    RendObj_AsciChar* c = &obj->data.asciChar;
    c->asciChar = asciCharacter;
}

void RendObj_SetAsAsciCharArray(RendObj* obj, char* asciCharArray, i32 numChars, f32 size)
{
    obj->type = RENDOBJ_TYPE_ASCI_CHAR_ARRAY;
    RendObj_AsciCharArray* c = &obj->data.charArray;
    c->chars = asciCharArray;
    c->numChars = numChars;
    c->size = size;
}

void RendObj_CalculateSpriteAsciUVs(
    RendObj_Sprite* sprite,
    u8 asciChar)
{
    // asci char sheet will be 256 characters, 16x16
    f32 strideX = 1.0f / 16;
    f32 strideY = 1.0f / 16;
    
    i32 sheetX = asciChar % 16;
    i32 sheetY = asciChar / 16;
    // Sheet is top -> down but opengl is bottom -> up so flip the Y coord
    sheetY = (16 - 1) - sheetY;

    sprite->uvLeft = (f32)strideX * (f32)sheetX;
    sprite->uvRight = (f32)sprite->uvLeft + (f32)strideX;

    sprite->uvBottom = (f32)strideY * (f32)sheetY;
    sprite->uvTop = (f32)sprite->uvBottom + (f32)strideY;
}
