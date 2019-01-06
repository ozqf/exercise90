#pragma once

#include "com_module.h"

// 2 to the power of N bits - 1:
#define SIX_BIT_MASK 63
#define TEN_BIT_MASK 1023
#define SIXTEEN_BIT_MASK 65535
#define TWENTY_FOUR_BIT_MASK 16777215

u8 COM_IsFlagDifferent(u32 flagsA, u32 flagsB, u32 flag);
void COM_DisableBits(u32* flags, u32 mask);
// Used to generate the mask constants used above
i32 COM_CreateBitmask(i32 numBits);
/**
 * Pack a 3D normal vector into an int with 10 bit precision
 * + 1 to make the number positive
 * multiple by 500 to move each vector component into
 * the range 0 - 1000)
*/
i32 COM_PackVec3NormalToI32(f32 x, f32 y, f32 z);
i32 COM_PackVec3NormalToI32(f32* vector3);
Vec3 COM_UnpackVec3Normal(i32 source);
