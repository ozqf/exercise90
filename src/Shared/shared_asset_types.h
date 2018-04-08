#pragma once

#include "shared_types.h"


struct Texture2DHeader
{
	i32 index;
	char name[32];
	i32 width;
	i32 height;
	u32 *ptrMemory;
};
