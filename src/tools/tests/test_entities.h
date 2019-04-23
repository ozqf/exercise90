#pragma once

#include "../../common/common.h"

struct Test_SimEntityType
{
    i32 factoryIndex;
    i32 thinkIndex;
    i32 thinkTime;

    i32 staticFlags;
    Vec3 scale;

    Colour colourA;
    Colour colourB;
};

struct Test_SimEntity
{
    Transform t;
    i32 lastThink;
    i32 nextThink;
    u8 thinkIndex;
    i32 activeFlags;
    i32 typeIndex;
    
    // cached for quick access, but located via typeIndex
    Test_SimEntityType* ptrType;
};

struct Test_SimSyncFields
{
	i32 id;
	i32 fields;     // maximum of 32
	float posX; 	// 0
	float posY; 	// 1
	float posZ; 	// 2
	float velX; 	// 3
	float velY; 	// 4
	float velZ; 	// 5
	float pitch; 	// 6
	float yaw; 		// 7
	
	u8 thinkIndex;	// 8
	i32 nextThink;	// 9
};
