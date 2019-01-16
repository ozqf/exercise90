#pragma once

void    Tex_Init(Heap* heap, PlatformInterface platform);
i32     Tex_RenderModuleReloaded();
void    Tex_DebugPrint();
i32     Tex_GetTextureIndexByName(char* textureName);
