#pragma once

#include "../com_defines.h"

struct BW8x8Block
{
  // bit == x
  // index == y
  unsigned char pixels[8];
};

struct BWImage
{
  int numBlocksX;
  int numBlocksY;
  unsigned char *blocks;
};

void Tex_GenerateBW(Texture2DHeader* h);

void COMTex_BMP2Internal(
    u32* sourcePixels,
    u32* destPixels,
    u32 numPixels);
