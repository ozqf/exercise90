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
  Point size;
  BW8x8Block* blocks;
};

void    TexDraw_Outline(Texture2DHeader* tex, ColourU32 col);
void    TexDraw_Line(
          Texture2DHeader* tex,
          ColourU32 col,
          i32 x0, i32 y0, i32 x1, i32 y1);

void 	  COMTex_SetAllPixels(Texture2DHeader* tex, ColourU32 col);
Point     Tex_CalcInternalImageSizeFromBW(BWImage* img);
Point     Tex_CalcBWImageSizeFromBitmap(Texture2DHeader* h);
void      Tex_BW2Bitmap(BWImage* img, Texture2DHeader* h);
void      Tex_BWSetAllPixels(BW8x8Block* block);
void      Tex_GenerateBW(Texture2DHeader* h, BWImage* dest);
void      COMTex_BMP2Internal(
            u32* sourcePixels,
            u32* destPixels,
            u32 numPixels);
