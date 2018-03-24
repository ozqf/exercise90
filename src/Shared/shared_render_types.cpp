#pragma once

#include "shared.h"

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

void RendObj_SetAsSprite(RendObj* obj,
    i32 mode, i32 textureIndex,
    f32 width, f32 height)
{
    obj->type = RENDOBJ_TYPE_SPRITE;
    RendObj_Sprite* rend = &obj->obj.sprite;
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
    RendObj_Billboard* rend = &obj->obj.billboard;
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
    RendObj_AsciChar* c = &obj->obj.asciChar;
    c->asciChar = asciCharacter;
}

void RendObj_SetAsAsciCharArray(RendObj* obj, char* asciCharArray, i32 numChars, f32 size)
{
    obj->type = RENDOBJ_TYPE_ASCI_CHAR_ARRAY;
    RendObj_AsciCharArray* c = &obj->obj.charArray;
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