#pragma once

#include "com_module.h"

#define RENDOBJ_FLAG_DEBUG (1 << 0)
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

static inline void RendObj_SetAsMesh(RendObj* obj, Mesh* mesh, f32 red, f32 green, f32 blue, i32 textureIndex);
static inline void RendObj_SetAsRainbowQuad(RendObj* obj);
static inline void RendObj_SetAsAABB(RendObj* obj, f32 sizeX, f32 sizeY, f32 sizeZ, f32 red, f32 green, f32 blue);
static inline void RendObj_SetAsColouredQuad(RendObj* obj, f32 red, f32 green, f32 blue);
static inline void RendObj_SetAsSprite(RendObj* obj, i32 mode, i32 textureIndex, f32 width, f32 height);
static inline void RendObj_SetAsLine(RendObj* obj, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, f32 r0, f32 g0, f32 b0, f32 r1, f32 g1, f32 b1);
static inline void RendObj_SetSpriteUVs(RendObj_Sprite* sprite, f32 uvLeft, f32 uvRight, f32 uvBottom, f32 uvTop);
static inline void RendObj_SetAsBillboard(RendObj* obj, f32 r, f32 g, f32 b, i32 textureIndex);
static inline void RendObj_SetAsAsciChar(RendObj* obj, u8 asciCharacter);
static inline void RendObj_SetAsAsciCharArray(RendObj* obj, char* asciCharArray, i32 numChars, f32 size, i32 textureIndex, f32 red, f32 green, f32 blue);
static inline void RendObj_CalculateSpriteAsciUVs(RendObj_Sprite* sprite, u8 asciChar);
