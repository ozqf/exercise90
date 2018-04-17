#pragma once

#include "../../Shared/shared_types.h"

//////////////////////////////////////////////////////////////////
// Collision Shapes
//////////////////////////////////////////////////////////////////
struct Collider_Cuboid
{
	int w;
    int h;
};

struct Collider_Sphere
{
	int radius;
};

struct Collider_Mesh
{
	unsigned  char* mesh;
};


union Collider_U
{
	Collider_Cuboid cuboid;
	Collider_Sphere sphere;
	Collider_Mesh mesh;
};

#define CAST_CUBOID(obj) &obj->data.cuboid;

struct Phys_Shape
{
	 int flags;
	 int type;
	 Collider_U data;
};

//////////////////////////////////////////////////////////////////
// Collision query data
//////////////////////////////////////////////////////////////////

struct ZRayHit
{
	Vec3 pos;
	Vec3 normal;
	Vec3 penetration;
	Phys_Shape* victim;
};

struct ZRayHitscan
{
	int searchId;
	ZRayHit hits[100];
};

struct ZVolumeSearch
{
	int searchId;
	Phys_Shape* shapes[100];
};

struct ZTouchPair
{
	Phys_Shape* a;
	Phys_Shape* b;
};
