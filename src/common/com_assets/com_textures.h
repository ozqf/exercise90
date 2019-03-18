#pragma once

#include "../com_module.h"

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
  i32 totalBytes;
  BW8x8Block* blocks;
};

i32       Tex_CalcBWImageSizeFromBitmap(Texture2DHeader* h, BWImage* img);
void      Tex_GenerateBW(Texture2DHeader* h, BWImage* dest);

void      COMTex_BMP2Internal(
            u32* sourcePixels,
            u32* destPixels,
            u32 numPixels);
