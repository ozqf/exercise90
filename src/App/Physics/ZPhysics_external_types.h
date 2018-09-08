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
#define ZCOLLIDER_FLAG_GROUNDCHECK (1 << 3)

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
	i32 tag;
	u16 group;
	u16 mask;
	f32 restitution;
	f32 pos[3];
	ZShapeData_U data;

	void SetAsBox(
		f32 x,
		f32 y,
		f32 z,
		f32 halfSizeX,
    	f32 halfSizeY,
    	f32 halfSizeZ,
    	u32 newFlags,
    	u16 newGroup,
    	u16 newMask,
		i32 newTag
	)
	{
		*this = {};
    	shapeType = box;
    	flags = newFlags;
    	group = newGroup;
    	mask = newMask;
		tag = newTag;
    	pos[0] = x;
    	pos[1] = y;
    	pos[2] = z;
    	data.box.halfSize[0] = halfSizeX;
    	data.box.halfSize[1] = halfSizeY;
    	data.box.halfSize[2] = halfSizeZ;
	}
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
	f32 start[3];
	f32 end[3];
	u16 group;
	u16 mask;
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
	RaycastDebug = 3,
	OverlapStarted = 4,
	OverlapEnded = 5
};
#define PHYS_UPDATE_NULL 0
#define PHYS_EVENT_TRANSFORM 1
#define PHYS_EVENT_RAYCAST 2

#define PHYS_EV_FLAG_GROUNDED (1 << 0)

struct PhysEV_TransformUpdate
{
	//i32 type;
	u32 ownerId;
	f32 matrix[16];
	f32 rot[3];
	f32 vel[3];
	f32 angularVel[3];
	u32 flags;
	i32 tag;
};

struct PhysRayHit
{
	f32 worldPos[3];
	f32 normal[3];
	i32 shapeId;
};

struct PhysEv_RaycastResult
{
	i32 queryId;
	f32 start[3];
	f32 end[3];
	i32 numHits;
};

struct PhysEv_RaycastDebug
{
	//i32 type;
	f32 a[3];
	f32 b[3];
	f32 colour[3];
};

struct PhysEv_Collision
{
	u32 shapeA;
	u32 shapeB;
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
