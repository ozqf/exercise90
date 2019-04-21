#pragma once

#include "../common.h"

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

// B&W encoding (for embedding textures)
i32     Tex_CalcBytesForBWPixels(i32 sourceWidth, i32 sourceHeight);
void    Tex_RGBA2BW(Texture2DHeader* tex, u8* target);
void    Tex_BW2BGBA(
  u8* source, Texture2DHeader* tex, ColourU32 off, ColourU32 on);
void    Tex_BWSetAllPixels(BW8x8Block* block);

void    TexDraw_Outline(Texture2DHeader* tex, ColourU32 col);
void    TexDraw_Gradient(Texture2DHeader* tex, i32 type);
void    TexDraw_FillRect(
          Texture2DHeader* tex, Point topLeft, Point size, ColourU32 col);
void    TexDraw_Line(
          Texture2DHeader* tex,
          ColourU32 col,
          i32 x0, i32 y0, i32 x1, i32 y1);

void 	    TexDraw_SetAllPixels(Texture2DHeader* tex, ColourU32 col);

Point     Tex_CalcInternalImageSizeFromBW(BWImage* img);
Point     Tex_CalcBWImageSizeFromBitmap(Texture2DHeader* h);

void      Tex_GenerateBW(Texture2DHeader* h, BWImage* dest);
void      COMTex_BMP2Internal(
            u32* sourcePixels,
            u32* destPixels,
            u32 numPixels);
com_internal i32 Tex_CalcBitmapMemorySize(i32 width, i32 height);
com_internal Texture2DHeader* Tex_SetTextureHeader(
              char* name, i32 width, i32 height, u8* memory);
