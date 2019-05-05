#pragma once

#include "../common.h"


///////////////////////////////////////////////////////////////////
// Build a render scene
///////////////////////////////////////////////////////////////////

com_internal void RScene_AddRenderItem(
    RenderScene*    scene,
    RendObj*        rendObj,
    f32 x, f32 y, f32 z,
    f32 scaleX, f32 scaleY, f32 scaleZ)
{
    COM_ASSERT(scene->numObjects < scene->maxObjects, "No capacity for render item");
    RenderListItem* item = &scene->sceneItems[scene->numObjects];
    scene->numObjects++;
    Transform_SetToIdentity(&item->transform);
    item->transform.pos.x = x;
    item->transform.pos.y = y;
    item->transform.pos.z = z;
    item->transform.scale.x = scaleX;
    item->transform.scale.y = scaleY;
    item->transform.scale.z = scaleZ;
    item->obj = *rendObj;
}

com_internal void RScene_AddRenderItem(RenderScene* scene, Transform* t, RendObj* rendObj)
{
    COM_ASSERT(scene->numObjects < scene->maxObjects, "No capacity for render item");
    RenderListItem* item = &scene->sceneItems[scene->numObjects];
    scene->numObjects++;
    item->transform = *t;
    item->obj = *rendObj;
}


com_internal void RScene_Init(RenderScene *scene, RenderListItem *objectArray, u32 maxObjects,
                  i32 fov, i32 projectionMode, f32 orthographicHalfHeight)
{
    *scene = {};
    Transform_SetToIdentity(&scene->cameraTransform);
    scene->numObjects = 0;
    scene->maxObjects = maxObjects;
    scene->sceneItems = objectArray;
    scene->settings.fov = fov;
    scene->settings.projectionMode = projectionMode;
    scene->settings.orthographicHalfHeight = orthographicHalfHeight;
}

// Default init
com_internal void RScene_Init(RenderScene *scene, RenderListItem *objectArray, u32 maxObjects)
{
    RScene_Init(scene, objectArray, maxObjects, 90, RENDER_PROJECTION_MODE_3D, 8);
}


///////////////////////////////////////////////////////////////////
// Init Render Objects
///////////////////////////////////////////////////////////////////

com_internal void RendObj_InterpolatePosition(
    Vec3* result, Vec3* origin, Vec3* dest, f32 percentage)
{
    f32 diffX = dest->x - origin->x;
    f32 diffY = dest->y - origin->y;
    f32 diffZ = dest->z - origin->z;
    result->x = origin->x + (diffX * percentage);
    result->y = origin->y + (diffY * percentage);
    result->z = origin->z + (diffZ * percentage);
}

com_internal void RendObj_SetAsMesh(
    RendObj* obj, MeshData mesh,
    Colour colour,
    i32 textureIndex)
{
    obj->type = RENDOBJ_TYPE_MESH;

    RendObj_ColouredMesh* rend = &obj->data.mesh;
    rend->mesh = mesh;
    rend->r = colour.r;
    rend->g = colour.g;
    rend->b = colour.b;
    rend->textureIndex = textureIndex;   
}

com_internal void RendObj_SetAsRainbowQuad(RendObj* obj)
{
    obj->type = RENDOBJ_TYPE_PRIMITIVE;
    
    RendObj_Primitive* prim = &obj->data.primitive;
    prim->primitiveType = REND_PRIMITIVE_TYPE_RAINBOW_QUAD;
}

com_internal void RendObj_SetAsAABB(RendObj* obj, f32 sizeX, f32 sizeY, f32 sizeZ,
    f32 red, f32 green, f32 blue)
{
    obj->type = RENDOBJ_TYPE_PRIMITIVE;
    
    RendObj_Primitive* prim = &obj->data.primitive;
    prim->primitiveType = REND_PRIMITIVE_TYPE_AABB;
    prim->size = { sizeX, sizeY, sizeZ };
    prim->colour.red = red;
    prim->colour.green = green;
    prim->colour.blue = blue;
}

com_internal void RendObj_SetAsColouredQuad(
    RendObj* obj, Colour colour)
{
    obj->type = RENDOBJ_TYPE_PRIMITIVE;

    RendObj_Primitive* prim = &obj->data.primitive;
    prim->primitiveType = REND_PRIMITIVE_TYPE_SINGLE_COLOUR_QUAD;
    prim->colour = colour;
}

com_internal void RendObj_SetAsSprite(RendObj* obj,
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

com_internal void RendObj_SetAsLine(
    RendObj* obj,
    Vec3 origin,
    Vec3 destination,
    Colour colourA,
    Colour colourB
    )
{
    obj->type = RENDOBJ_TYPE_LINE;
    RendObj_Line* rend = &obj->data.line;
    rend->a = origin;
    rend->b = destination;

    rend->colourA = colourA;
    rend->colourB = colourB;
}

com_internal void RendObj_SetSpriteUVs(
    RendObj_Sprite* sprite,
    f32 uvLeft, f32 uvRight, f32 uvBottom, f32 uvTop)
{
    sprite->uvLeft = uvLeft;
    sprite->uvRight = uvRight;
    sprite->uvBottom = uvBottom;
    sprite->uvTop = uvTop;
}

com_internal void RendObj_SetAsBillboard(RendObj* obj, f32 r, f32 g, f32 b, i32 textureIndex)
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

com_internal void RendObj_SetAsAsciChar(RendObj* obj, u8 asciCharacter)
{
    obj->type = RENDOBJ_TYPE_ASCI_CHAR;
    RendObj_AsciChar* c = &obj->data.asciChar;
    c->asciChar = asciCharacter;
}

com_internal void RendObj_SetAsAsciCharArray(
    RendObj* obj,
    char* asciCharArray,
    i32 numChars,
    f32 size,
    i32 alignmentMode,
    i32 textureIndex,
    f32 red, f32 green, f32 blue
    )
{
    obj->type = RENDOBJ_TYPE_ASCI_CHAR_ARRAY;
    RendObj_AsciCharArray* c = &obj->data.charArray;
    
    c->chars = asciCharArray;
    c->numChars = numChars;
    c->size = size;
    c->alignmentMode = alignmentMode;
    c->textureIndex = textureIndex;
    c->r = red;
    c->g = green;
    c->b = blue;
}

com_internal void RendObj_CalculateSpriteAsciUVs(
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

