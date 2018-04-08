#pragma once

#include "../Shared/shared.h"

struct RenderInterface
{
    void (*R_Init)();

    void (*R_BindTexture)(void* rgbaPixels, u32 width, u32 height, u32 textureIndex);

    void (*R_SetupFrame)();
    void (*R_FinishFrame)();
    void (*R_RenderScene)(RenderScene* scene, GameTime* time);
};
