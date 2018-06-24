#pragma once

#include "../../common/com_defines.h"

//////////////////////////////////////////////////////////////////
// Collision Shapes
//////////////////////////////////////////////////////////////////
#define ZCOLLIDER_TYPE_CUBOID 0
#define ZCOLLIDER_TYPE_SPHERE 1
#define ZCOLLIDER_TYPE_CAPSULE 2
#define ZCOLLIDER_TYPE_MESH 3

enum ZShapeType
{
	box = 0,
	sphere = 1,
	capsule = 2,
	mesh = 3
};
//#define ZCOLLIDER_TYPE_INFINITE_PLANE 99

#define ZCOLLIDER_FLAG_STATIC (1 << 0)
#define ZCOLLIDER_FLAG_KINEMATIC (1 << 1)
#define ZCOLLIDER_FLAG_NO_ROTATION (1 << 2)
#if 0

struct ZShapeDef
{
	u32 type;
	u32 flags;
	u16 mask;
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

#if 1

struct ZCollisionBoxData
{
	f32 halfSize[3];
};

struct ZCollisionSphereData
{
	f32 radius;
};
#if 0
struct ZCollisionCapsuleData
{
	f32 halfWidth;
	f32 halfHeight;
};

struct ZCollisionMeshData
{
	f32 halfWidth;
	f32 halfHeight;
};
#endif
union ZShapeData_U
{
	ZCollisionBoxData box;
	ZCollisionSphereData sphere;
	//ZCollisionCapsuleData capsule;
	//ZCollisionMeshData mesh;
};

struct ZShapeDef
{
	ZShapeType shapeType;
	
	u32 flags;
	i32 handleId;
	u16 group;
	u16 mask;
	f32 pos[3];
	ZShapeData_U data;
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
#if 1
struct PhysCmd_Teleport
{
	// u8 type
	i32 shapeId;
	f32 pos[3];
};

struct PhysCmd_VelocityChange
{
	// u8 type
	i32 shapeId;
	i32 mode = 0;
	f32 vel[3];
};

struct PhysCmd_Raycast
{
	i32 id;
	f32 origin[3];
	f32 dest[3];

};

#endif

//////////////////////////////////////////////////////////////////
// Events -> out going data from physics engine step
//////////////////////////////////////////////////////////////////

enum PhysEventType
{
	None = 0,
	TransformUpdate = 1,
	RaycastResult = 2,
	RaycastDebug = 3
};
#define PHYS_UPDATE_NULL 0
#define PHYS_EVENT_TRANSFORM 1
#define PHYS_EVENT_RAYCAST 2

#define PHYS_EV_FLAG_GROUNDED (1 << 0)

struct PhysEV_TransformUpdate
{
	//i32 type;
	u16 ownerId;
	u16 ownerIteration;
	f32 matrix[16];
	f32 rot[3];
	f32 vel[3];
	f32 angularVel[3];
	u32 flags;
};

struct PhysEv_RaycastResult
{
	//i32 type;
	f32 a[3];
	f32 b[3];
	f32 colour[3];
};

struct PhysEv_RaycastDebug
{
	//i32 type;
	f32 a[3];
	f32 b[3];
	f32 colour[3];
};

//////////////////////////////////////////////////////////////////
// Header for all items in event or command buffers
//////////////////////////////////////////////////////////////////
struct PhysDataItemHeader
{
	PhysEventType type;
    i32 size;
};

struct PhysCmd_State
{
	i32 shapeId;
	f32 pos[3];
	f32 rot[3];
	f32 vel[3];
};
