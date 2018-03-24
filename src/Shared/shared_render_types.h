#pragma once

#include "../Shared/shared.h"

#define RENDOBJ_TYPE_NONE 0
#define RENDOBJ_TYPE_PRIMITIVE 1
#define RENDOBJ_TYPE_BILLBOARD 2
#define RENDOBJ_TYPE_MESH 3
#define RENDOBJ_TYPE_LINE 4
#define RENDOBJ_TYPE_SPRITE 5
#define RENDOBJ_TYPE_ASCI_CHAR 6
#define RENDOBJ_TYPE_ASCI_CHAR_ARRAY 7

#define SPRITE_MODE_MESH 0
#define SPRITE_MODE_BILLBOARD 1
#define SPRITE_MODE_UI 2

#define REND_PRIMITIVE_TYPE_RAINBOW_QUAD 0
#define REND_PRIMITIVE_TYPE_SINGLE_COLOUR_QUAD 1

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
    f32 uvLeft;
    f32 uvRight;
    f32 uvBottom;
    f32 uvTop;
    f32 r;
    f32 g;
    f32 b;
    f32 a;
};

struct RendObj_Sprite
{
    i32 mode;
    i32 textureIndex;
    f32 width;
    f32 height;
    f32 uvLeft;
    f32 uvRight;
    f32 uvBottom;
    f32 uvTop;
    f32 r;
    f32 g;
    f32 b;
    f32 a;
};

// struct RendObj_Line
// {
//     Vec3 a;
//     Vec3 b;
//     Vec3 colourA;
//     Vec3 colourB;
// };

struct RendObj_AsciChar
{
    u8 asciChar;
};

struct RendObj_AsciCharArray
{
    char* chars;
    i32 numChars;
    f32 size;
};

struct RendObj_ColouredMesh
{
    Mesh* mesh;
    f32 r, g, b;
    i32 textureIndex;
};

union RendObj_Union
{
    RendObj_ColouredMesh mesh;
    RendObj_Sprite sprite;
    RendObj_Primitive primitive;

    // TODO: Remove these, billboard replced by sprite, ascichar removed
    RendObj_Billboard billboard;
    RendObj_AsciChar asciChar;
    RendObj_AsciCharArray charArray;
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