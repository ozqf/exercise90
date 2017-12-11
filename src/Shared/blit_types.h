#ifndef BLIT_TYPES_H
#define BLIT_TYPES_H

#include "shared_types.h"

#define BLIT_ITEM_BLOCK 1
struct BlitBlock
{
    f32 centre[3];
    f32 halfWidth;
    f32 halfHeight;
    uChar red, green, blue;
};

#define BLIT_ITEM_IMAGE 2
struct BlitImage
{
    f32 centre[3];
    i32 imageIndex;
};

#define BLIT_ITEM_LINE 3
struct BlitLine
{
    f32 a[3];
    f32 b[3];
};

union BlitItemUnion
{
    BlitBlock block;
    BlitLine line;
    BlitImage image;
};

struct BlitItem
{
    i32 type;
    BlitItemUnion content;
};


#endif