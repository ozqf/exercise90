#pragma once

#include "ZPhysics_internal_types.h"

/*
COPYED FROM ORIGINAL BULLET SOURCE
- Missing in header files for lib
*/
	enum CollisionFlags
	{
		CF_STATIC_OBJECT= 1,
		CF_KINEMATIC_OBJECT= 2,
		CF_NO_CONTACT_RESPONSE = 4,
		CF_CUSTOM_MATERIAL_CALLBACK = 8,//this allows per-triangle material (friction/restitution)
		CF_CHARACTER_OBJECT = 16,
		CF_DISABLE_VISUALIZE_OBJECT = 32, //disable debug drawing
		CF_DISABLE_SPU_COLLISION_PROCESSING = 64,//disable parallel/SPU processing
		CF_HAS_CONTACT_STIFFNESS_DAMPING = 128,
		CF_HAS_CUSTOM_DEBUG_RENDERING_COLOR = 256,
		CF_HAS_FRICTION_ANCHOR = 512,
		CF_HAS_COLLISION_SOUND_TRIGGER = 1024
	};

	enum	CollisionObjectTypes
	{
		CO_COLLISION_OBJECT =1,
		CO_RIGID_BODY=2,
		///CO_GHOST_OBJECT keeps track of all objects overlapping its AABB and that pass its collision filter
		///It is useful for collision sensors, explosion objects, character controller etc.
		CO_GHOST_OBJECT=4,
		CO_SOFT_BODY=8,
		CO_HF_FLUID=16,
		CO_USER_TYPE=32,
		CO_FEATHERSTONE_LINK=64
	};

	enum AnisotropicFrictionFlags
	{
		CF_ANISOTROPIC_FRICTION_DISABLED=0,
		CF_ANISOTROPIC_FRICTION = 1,
		CF_ANISOTROPIC_ROLLING_FRICTION = 2
	};


PhysBodyHandle* PHYS_GetFreeBodyHandle(PhysBodyList* list);

void Phys_FreeHandle(ZBulletWorld* world, PhysBodyHandle* handle);

void Phys_RecycleHandle(ZBulletWorld* world, PhysBodyHandle* handle);

PhysBodyHandle* Phys_CreateBulletSphere(ZBulletWorld* world, ZSphereDef def);

PhysBodyHandle* Phys_CreateBulletBox(ZBulletWorld* world, ZBoxDef def);

PhysBodyHandle* Phys_CreateBulletInfinitePlane(ZBulletWorld* world, ZShapeDef def);

internal void Phys_PreSolveCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);

internal void Phys_PostSolveCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);

internal void Phys_WriteDebugOutput(ZBulletWorld* world);
