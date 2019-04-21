#pragma once

#include "../common/common.h"

#define MAX_TEXTURES 128
/*
Textures must be loaded and bound separately so that
they can be rebound when the renderer is reloaded
> Loading triggered by app
> binding called after texture loading or after renderer restart

Components:
> Opengl index -> opengl int: mapping for graphics card. May be different, and non sequential when created
> Custom index -> integer: app keeps track of the texture


*/

struct Assets_textureSet
{
    BlockRef textureRefs[MAX_TEXTURES];
    u32 numTextures = 0;
};
