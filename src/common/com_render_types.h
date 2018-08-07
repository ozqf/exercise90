#pragma once

#include "com_module.h"

#define RENDOBJ_FLAG_DEBUG (1 << 0)
//#define RENDOBJ_FLAG_DEBUG (1 << 0)


// identity == render directly to camera space.
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

#define TEXT_ALIGNMENT_TOP_LEFT -4
#define TEXT_ALIGNMENT_TOP_MIDDLE -3
#define TEXT_ALIGNMENT_TOP_RIGHT -2
#define TEXT_ALIGNMENT_MIDDLE_LEFT -1
#define TEXT_ALIGNMENT_MIDDLE_MIDDLE 0
#define TEXT_ALIGNMENT_MIDDLE_RIGHT 1
#define TEXT_ALIGNMENT_BOTTOM_LEFT 2
#define TEXT_ALIGNMENT_BOTTOM_MIDDLE 3
#define TEXT_ALIGNMENT_BOTTOM_RIGHT 4

#define SPRITE_MODE_MESH 0
#define SPRITE_MODE_BILLBOARD 1
#define SPRITE_MODE_UI 2

#define REND_PRIMITIVE_TYPE_RAINBOW_QUAD 0
#define REND_PRIMITIVE_TYPE_SINGLE_COLOUR_QUAD 1
#define REND_PRIMITIVE_TYPE_AABB 2

struct RendObj_Primitive;
struct RendObj_Billboard;
struct RendObj_Sprite;
struct RendObj_Line;
struct RendObj_AsciChar;
struct RendObj_AsciCharArray;
struct RendObj_ColouredMesh;
union RendObj_Union;
// Individual render item
struct RendObj;
struct RenderListItem;
// Scene container for render call each frame
struct RenderScene;


///////////////////////////////////////////////////////////////////
// Build a render scene
///////////////////////////////////////////////////////////////////

static inline void RScene_AddRenderItem(RenderScene* scene, Transform* t, RendObj* rendObj);
static inline void RScene_Init(RenderScene *scene, RenderListItem *objectArray, u32 maxObjects,
                  i32 fov, i32 projectionMode, f32 orthographicHalfHeight);
static inline void RScene_Init(RenderScene *scene, RenderListItem *objectArray, u32 maxObjects);

///////////////////////////////////////////////////////////////////
// Init Render Objects
///////////////////////////////////////////////////////////////////

static inline void RendObj_SetAsMesh(RendObj* obj, MeshData mesh, f32 red, f32 green, f32 blue, i32 textureIndex);
static inline void RendObj_SetAsRainbowQuad(RendObj* obj);
static inline void RendObj_SetAsAABB(RendObj* obj, f32 sizeX, f32 sizeY, f32 sizeZ, f32 red, f32 green, f32 blue);
static inline void RendObj_SetAsColouredQuad(RendObj* obj, f32 red, f32 green, f32 blue);
static inline void RendObj_SetAsSprite(RendObj* obj, i32 mode, i32 textureIndex, f32 width, f32 height);
static inline void RendObj_SetAsLine(RendObj* obj, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, f32 r0, f32 g0, f32 b0, f32 r1, f32 g1, f32 b1);
static inline void RendObj_SetSpriteUVs(RendObj_Sprite* sprite, f32 uvLeft, f32 uvRight, f32 uvBottom, f32 uvTop);
static inline void RendObj_SetAsBillboard(RendObj* obj, f32 r, f32 g, f32 b, i32 textureIndex);
static inline void RendObj_SetAsAsciChar(RendObj* obj, u8 asciCharacter);
static inline void RendObj_SetAsAsciCharArray(RendObj* obj, char* asciCharArray, i32 numChars, f32 size, i32 alignmentMode, i32 textureIndex, f32 red, f32 green, f32 blue);
static inline void RendObj_CalculateSpriteAsciUVs(RendObj_Sprite* sprite, u8 asciChar);



/****************************************************************
Graphics
****************************************************************/
struct MeshData
{
	u32 numVerts;

	f32* verts;
	f32* uvs;
    f32* normals;
    
};

struct Texture2DHeader
{
	i32 index;
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
    i32 textureIndex;
};

struct RendObj_AsciCharArray
{
    char* chars;
    i32 numChars;
    f32 size;
    i32 alignmentMode;
    f32 r;
    f32 g;
    f32 b;
    i32 textureIndex;
};

struct RendObj_ColouredMesh
{
    MeshData mesh;
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
    u16 type;
    u32 flags;
    RendObj_Union data;

    void SetColour(f32 r, f32 g, f32 b)
    {
        switch (this->type)
        {
            case RENDOBJ_TYPE_PRIMITIVE:
            {
                this->data.primitive.red = r;
                this->data.primitive.green = g;
                this->data.primitive.blue = b;
            }  break;

            case RENDOBJ_TYPE_BILLBOARD:
            {
                this->data.billboard.r = r;
                this->data.billboard.g = g;
                this->data.billboard.b = b;
            } break;

            case RENDOBJ_TYPE_ASCI_CHAR_ARRAY:
            {
                this->data.charArray.r = r;
                this->data.charArray.g = g;
                this->data.charArray.b = b;
            } break;
        }
    }
};

struct RenderListItem
{
    Transform transform;
    RendObj obj;
};

#define VIEW_MODEL_MODE_EULER 0
#define VIEW_MODEL_MODE_DIRECT 1

struct RenderSceneSettings
{
    i32 projectionMode = 0;
    f32 orthographicHalfHeight = 8;
    i32 fov = 90;
    i32 viewModelMode = 0;
};

// Scene container for render call each frame
struct RenderScene
{    
    Transform cameraTransform;
    
    RenderSceneSettings settings;

    RenderListItem* sceneItems;
    u32 numObjects;
    u32 maxObjects;
};

struct InputItem
{
    char on = 0;
    u32 lastChangeFrame = 0;
};

struct ScreenInfo
{
    i32 width;
    i32 height;
    f32 aspectRatio;
};
