#pragma once

#include "game.h"
#if 0
//////////////////////////////////////////////////////
// Types associated with the Entity Component System
//////////////////////////////////////////////////////


#define EC_FLAG_TRANSFORM (1 << 0)
#define EC_FLAG_COLLIDER (1 << 1)
#define EC_FLAG_RENDERER (1 << 2)
//#define EC_FLAG_ACTORMOTOR (1 << 3)
//#define EC_FLAG_PROJECTILE (1 << 4)
//#define EC_FLAG_BRAIN (1 << 5)
//#define EC_FLAG_LABEL (1 << 6)
//#define EC_FLAG_HEALTH (1 << 7)

struct EC_Header
{
    EntId entId;
    u8 inUse;
};

struct EC_Transform
{
    EC_Header header;

    Transform t;
};

#define EC_RENDERER_STRING_LENGTH 32

struct EC_RendererState
{
    char meshName[32];
    char textureName[32];

};

struct EC_Renderer
{
    EC_Header header;

    RendObj rendObj;

    EC_RendererState state;
};

struct EC_LabelState
{
    char label[32];
};

struct EC_Label2
{
    EC_Header header;
};

#if 0
struct EC_ColliderState
{
    u32 lastFrameOverlapping;
    Vec3 size;
    Vec3 velocity;
};

struct EC_Collider
{
    EC_Header header;

    i32 shapeId;
    i32 isOverlapping;
    u8 isGrounded;

    EC_ColliderState state;
};
#endif

// A full, no nonsense description of
// an entity with all of it's components
struct EntityState
{
    EntId entId;
    // As this contains every possible state struct
    // use flags to know which should actually be used!
    u32 componentFlags;
    Transform t;
    EC_RendererState renderState;
    EC_LabelState labelState;
    // note: size so far is 164 bytes
};
#endif
