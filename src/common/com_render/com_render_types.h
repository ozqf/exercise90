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

union Colour;
union ColourU32;
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

struct Texture2DHeader;
typedef Texture2DHeader* Com_AllocateTexture(char* name, i32 width, i32 height);


/****************************************************************
Graphics
****************************************************************/

union Colour
{
	f32 array[4];
	struct
	{
		f32 red, green, blue, alpha;
	};
    struct
	{
		f32 r, g, b, a;
	};
};

union ColourU32
{
    struct
    {
        u32 value;
    };
    
	u8 array[4];
	struct
	{
		u8 red, green, blue, alpha;
	};
    struct
	{
		u8 r, g, b, a;
	};
};

struct MeshData
{
	u32 numVerts;

	f32* verts;
	f32* uvs;
    f32* normals;    
};

#define TEXTURE2D_MAX_NAME_LENGTH 31
struct Texture2DHeader
{
	i32 index;
	char name[(TEXTURE2D_MAX_NAME_LENGTH + 1)];
	i32 width;
	i32 height;
	u32 *ptrMemory;
};



struct RendObj_Primitive
{
    i32 primitiveType;
    Colour colour;
    Vec3 size;
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
    Colour colourA;
    Colour colourB;
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
    Vec3 currentPosition;
    Vec3 previousPosition;
    RendObj_Union data;

    void SetColour(f32 r, f32 g, f32 b)
    {
        switch (this->type)
        {
            case RENDOBJ_TYPE_MESH:
            {
                this->data.mesh.r = r;
                this->data.mesh.g = g;
                this->data.mesh.b = b;
            }  break;

            case RENDOBJ_TYPE_PRIMITIVE:
            {
                this->data.primitive.colour.r = r;
                this->data.primitive.colour.g = g;
                this->data.primitive.colour.b = b;
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

#define RENDERER_MAX_LIGHTS 8

struct SimpleLight
{
	i32 state;				// if 0, ignore
	f32 rgba[4];
	Vec3 worldPosition;
	i32 intensity;
};

struct RenderSceneSettings
{
	Transform cameraTransform;
    i32 projectionMode = 0;
    f32 orthographicHalfHeight = 8;
    i32 fov = 90;
    i32 viewModelMode = 0;
    u8 lightBits;
	SimpleLight lights[RENDERER_MAX_LIGHTS];
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

struct ScreenInfo
{
    i32 width;
    i32 height;
    f32 aspectRatio;
};


///////////////////////////////////////////////////////////////////
// Build a render scene
///////////////////////////////////////////////////////////////////

com_internal void RScene_AddRenderItem(RenderScene* scene, Transform* t, RendObj* rendObj);
com_internal void RScene_Init(RenderScene *scene, RenderListItem *objectArray, u32 maxObjects,
                  i32 fov, i32 projectionMode, f32 orthographicHalfHeight);
com_internal void RScene_Init(RenderScene *scene, RenderListItem *objectArray, u32 maxObjects);

///////////////////////////////////////////////////////////////////
// Init Render Objects
///////////////////////////////////////////////////////////////////

com_internal void RendObj_SetAsMesh(RendObj* obj, MeshData mesh, Colour colour, i32 textureIndex);
com_internal void RendObj_SetAsRainbowQuad(RendObj* obj);
com_internal void RendObj_SetAsAABB(RendObj* obj, f32 sizeX, f32 sizeY, f32 sizeZ, f32 red, f32 green, f32 blue);
com_internal void RendObj_SetAsColouredQuad(RendObj* obj, Colour colour);
com_internal void RendObj_SetAsSprite(RendObj* obj, i32 mode, i32 textureIndex, f32 width, f32 height);
com_internal void RendObj_SetAsLine(RendObj* obj, Vec3 origin, Vec3 destination, Colour colourA, Colour colourB);
com_internal void RendObj_SetSpriteUVs(RendObj_Sprite* sprite, f32 uvLeft, f32 uvRight, f32 uvBottom, f32 uvTop);
com_internal void RendObj_SetAsBillboard(RendObj* obj, f32 r, f32 g, f32 b, i32 textureIndex);
com_internal void RendObj_SetAsAsciChar(RendObj* obj, u8 asciCharacter);
com_internal void RendObj_SetAsAsciCharArray(RendObj* obj, char* asciCharArray, i32 numChars, f32 size, i32 alignmentMode, i32 textureIndex, f32 red, f32 green, f32 blue);
com_internal void RendObj_CalculateSpriteAsciUVs(RendObj_Sprite* sprite, u8 asciChar);
com_internal void RendObj_InterpolatePosition(Vec3* result, Vec3* origin, Vec3* dest, f32 percentage);
