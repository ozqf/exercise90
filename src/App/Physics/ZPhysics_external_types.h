#pragma once

#include "../../Shared/shared_types.h"

//////////////////////////////////////////////////////////////////
// Collision Shapes
//////////////////////////////////////////////////////////////////
#define ZCOLLIDER_TYPE_SPHERE 0
#define ZCOLLIDER_TYPE_CUBOID 1
#define ZCOLLIDER_TYPE_MESH 2

struct ZCuboidCollider
{
	f32 size[3];
};

struct ZSphereCollider
{
	f32 radius;
};

struct ZMeshCollider
{
	unsigned  char* mesh;
};


union ZCollider_U
{
	ZCuboidCollider cuboid;
	ZSphereCollider sphere;
	ZMeshCollider mesh;
};

#define CAST_CUBOID(obj) &obj->data.cuboid;

struct ZCollider
{
	 int flags;
	 int type;
	 ZCollider_U data;
};

//////////////////////////////////////////////////////////////////
// Collision query data
//////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////
// Collision query data
//////////////////////////////////////////////////////////////////

struct ZTransformUpdateEvent
{
	i32 type;
	u16 ownerId;
	u16 ownerIteration;
	Vec3 pos;
};
