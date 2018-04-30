#pragma once

#include "../Shared/shared.h"

const u32 RENDOBJ_FLAG_DEBUG = 1 << 0; // 1
//#define RENDOBJ_FLAG_DEBUG (1 << 0)

#define RENDER_PROJECTION_MODE_3D 0
#define RENDER_PROJECTION_MODE_ORTHOGRAPHIC 1
#define RENDER_PROJECTION_MODE_IDENTITY 2
#define RENDER_PROJECTION_MODE_3D_OLD 3

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
#define REND_PRIMITIVE_TYPE_AABB 2

struct RendObj_Primitive
{
    i32 primitiveType;
    f32 red;
    f32 green;
    f32 blue;
    f32 alpha;
    f32 sizeX;
    f32 sizeY;
    f32 sizeZ;
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

struct RendObj_AsciCharArray
{
    char* chars;
    i32 numChars;
    f32 size;
    f32 r;
    f32 g;
    f32 b;
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
    RendObj_Line line;

    // TODO: Remove these, billboard replced by sprite, ascichar removed
    RendObj_Billboard billboard;
    RendObj_AsciChar asciChar;
    RendObj_AsciCharArray charArray;
};

// Individual render item
struct RendObj
{
    u16 ownerIndex;
    u16 inUse;
    u16 type;
    u32 flags;
    RendObj_Union data;
};

struct RenderListItem
{
    Transform transform;
    RendObj obj;
};

// Scene container for render call each frame
struct RenderScene
{    
    Transform cameraTransform;
    AngleVectors cameraAngleVectors;

    i32 projectionMode;
    f32 orthographicHalfHeight;
    i32 fov;

    RenderListItem* sceneItems;
    u32 numObjects;
    u32 maxObjects;
};
