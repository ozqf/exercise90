#pragma once

#include "../../common/com_defines.h"

//////////////////////////////////////////////////////////////////
// Collision Shapes
//////////////////////////////////////////////////////////////////
#define ZCOLLIDER_TYPE_SPHERE 0
#define ZCOLLIDER_TYPE_CUBOID 1
#define ZCOLLIDER_TYPE_MESH 2
#define ZCOLLIDER_TYPE_INFINITE_PLANE 3

#define ZCOLLIDER_FLAG_STATIC (1 << 0)
#define ZCOLLIDER_FLAG_KINEMATIC (1 << 1)
#define ZCOLLIDER_FLAG_NO_ROTATION (1 << 2)
#if 1

struct ZShapeDef
{
	u32 type;
	u32 flags;
	f32 pos[3];
};

struct ZSphereDef
{
	ZShapeDef base;
	f32 radius;
};

struct ZBoxDef
{
	ZShapeDef base;
	f32 halfSize[3];
};

#endif
//////////////////////////////////////////////////////////////////
// Collision query data
//////////////////////////////////////////////////////////////////
#if 0
struct ZRayHit
{
	Vec3 pos;
	Vec3 normal;
	Vec3 penetration;
	ZCollider* victim;
};

struct ZRayHitscan
{
	int searchId;
	ZRayHit hits[100];
};

struct ZVolumeSearch
{
	int searchId;
	ZCollider* shapes[100];
};

struct ZTouchPair
{
	ZCollider* a;
	ZCollider* b;
};
#endif
//////////////////////////////////////////////////////////////////
// Commands -> incoming instructions to the physics engine
//////////////////////////////////////////////////////////////////
#if 0
struct ZTeleport
{
	i32 type;
	f32 pos[3];
};
#endif

//////////////////////////////////////////////////////////////////
// Events -> out going data from physics engine step
//////////////////////////////////////////////////////////////////
#if 1
struct ZTransformUpdateEvent
{
	i32 type;
	u16 ownerId;
	u16 ownerIteration;
	f32 matrix[16];
};
#endif
