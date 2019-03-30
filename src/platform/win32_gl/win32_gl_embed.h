#pragma once

#include "../../common/com_module.h"

internal void Win32_R_InitEmbeddedTextures(RendererPlatform* platform)
{
    #if 1
    Point bitmapSize = { 128, 128 };
    i32 totalBytes = Tex_CalcBitmapMemorySize(bitmapSize.x, bitmapSize.y);

    MemoryBlock mem = {};
    platform->Malloc(&mem, totalBytes);
    tex_charset = Tex_SetTextureHeader(
        TEX_CHARSET_NAME, bitmapSize.x, bitmapSize.y, (u8*)mem.ptrMemory);

    // Copy B&W to result
    Tex_BW2BGBA(
        Embed_GetCharset128x128BW(),
        tex_charset,
        { 255, 255, 255, 255 }, { 0, 0, 0, 255 });

    R_BindTexture(
        tex_charset->ptrMemory,
        tex_charset->width,
        tex_charset->height,
        EMBED_TEX_CHARSET_INDEX,
        1);
    #endif
}

internal i32 Win32_R_GetEmbeddedTextureIndex(char* name)
{
    if (!COM_CompareStrings(name, TEX_CHARSET_NAME))
    {
        return EMBED_TEX_CHARSET_INDEX;
    }
    return COM_ERROR_BAD_INDEX;
}
