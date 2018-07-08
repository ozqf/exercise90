#pragma once

#include "com_module.h"

#define TEN_BIT_MASK 1023
#define SIXTEEN_BIT_MASK 65535

// Used to generate the mask constants used above
inline i32 COM_CreateBitmask(i32 numBits)
{
    i32 m = 0;
    i32 bit = 0;
    while(bit < numBits)
    {
        m |= (1 << bit);
        bit++;
    }
    return m;
}

/**
 * Pack a 3D normal vector into an int with 10 bit precision
 * + 1 to make the number positive
 * multiple by 500 to move each vector component into
 * the range 0 - 1000)
*/
inline i32 COM_PackVec3NormalToI32(f32 x, f32 y, f32 z)
{
    i32 intX = (i32)((x + 1) * 500);
    i32 intY = (i32)((y + 1) * 500);
    i32 intZ = (i32)((z + 1) * 500);
    i32 r = 0;
    r |= (intX << 20);
    r |= (intY << 10);
    r |= (intZ << 0);
    return r;
}

inline i32 COM_PackVec3NormalToI32(f32* vector3)
{
    return COM_PackVec3NormalToI32(vector3[0], vector3[1], vector3[2]);
}

inline Vec3 COM_UnpackVec3Normal(i32 source)
{
    Vec3 r = {};
    //i32 mask = COM_CreateBitmask(10);
    i32 mask = TEN_BIT_MASK;
    i32 intX = ((mask << 20) & source) >> 20;
    i32 intY = ((mask << 10) & source) >> 10;
    i32 intZ = ((mask << 0) & source) >> 0;
    r.x = (f32)intX / 500;
    r.y = (f32)intY / 500;
    r.z = (f32)intZ / 500;
    r.x -= 1;
    r.y -= 1;
    r.z -= 1;
    return r;
}