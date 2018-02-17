#ifndef APP_MAIN_CPP
#define APP_MAIN_CPP

#include "../Shared/shared.h"
#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"

#include <stdio.h>

PlatformInterface platform;

GameObject player = {};

int numBlitItems = 0;
BlitItem blitItems[4096];

void ClearBlitItems()
{
    numBlitItems = 0;
}

void AppInit()
{
    printf("DLL Init\n");
    // Init player
    player.halfWidth = 1;
    player.halfHeight = 1;
    player.pos[0] = 0;
    player.pos[1] = 0;
    player.vel[0] = 0;
    player.vel[1] = 0;
    player.speed = 10;
}

void AppShutdown()
{
    printf("DLL Shutdown\n");
}

void AppFrame(GameTime* time, InputTick* input)
{
    //printf("APP FRAME: Ticks: %d\n", time.frameNumber);
    
    platform.PlatformClearScreen();
    ClearBlitItems();
    
    blitItems[numBlitItems].type = BLIT_ITEM_BLOCK;
    blitItems[numBlitItems].content.block.red = 0xFF;
    blitItems[numBlitItems].content.block.green = 0x00;
    blitItems[numBlitItems].content.block.blue = 0x00;
    blitItems[numBlitItems].content.block.centre[0] = player.pos[0] + 3;
    blitItems[numBlitItems].content.block.centre[1] = player.pos[1] + 1;
    blitItems[numBlitItems].content.block.halfWidth = player.halfWidth;
    blitItems[numBlitItems].content.block.halfHeight = player.halfHeight;
    numBlitItems++;

    blitItems[numBlitItems].type = BLIT_ITEM_BLOCK;
    blitItems[numBlitItems].content.block.red = 0x00;
    blitItems[numBlitItems].content.block.green = 0xFF;
    blitItems[numBlitItems].content.block.blue = 0x00;
    blitItems[numBlitItems].content.block.centre[0] = 12;
    blitItems[numBlitItems].content.block.centre[1] = 12;
    blitItems[numBlitItems].content.block.halfWidth = 4;
    blitItems[numBlitItems].content.block.halfHeight = 3;
    numBlitItems++;

    blitItems[numBlitItems].type = BLIT_ITEM_IMAGE;
    blitItems[numBlitItems].content.image.centre[0] = player.pos[0];
    blitItems[numBlitItems].content.image.centre[1] = player.pos[1];
    numBlitItems++;

    
    platform.PlatformRenderBlitItems(blitItems, numBlitItems);
}

void AppFixedFrame(GameTime* time, InputTick* inputTick)
{
    //printf("FIXED FRAME: FixedTicks: %d, Ticks %d, frames: %d, now: %I64d, last: %I64d, diff: %I64d, deltaTime: %f\n", time.fixedFrameNumber, time.ticks, time.frameNumber, now, last, diff, time.deltaTime);

    //printf("APP FIXED: FixedTicks: %d\n", time.fixedFrameNumber);

    //printf("Input: U: %d, D: %d, L: %d, R: %d\nb", inputTick->up, inputTick->down, inputTick->left, inputTick->right);

    f32 dt = time->fixedDeltaTime;
    player.vel[0] = 0;
    player.vel[1] = 0;
    player.vel[0] += (inputTick->right * player.speed) * dt;
    player.vel[0] -= (inputTick->left * player.speed) * dt;
    player.vel[1] -= (inputTick->up * player.speed) * dt;
    player.vel[1] += (inputTick->down * player.speed) * dt;

    // Update
    player.pos[0] += player.vel[0];
    player.pos[1] += player.vel[1];

    //printf("Player pos %f, %f\n", player.vel[0], player.vel[1]);

    f32 minX = platform.PlatformGetViewPortMinX();
    f32 minY = platform.PlatformGetViewPortMinY();
    f32 maxX = platform.PlatformGetViewPortMaxX();
    f32 maxY = platform.PlatformGetViewPortMaxY();

    if (player.pos[0] < minX)
    {
        player.pos[0] = maxX;
    }

    if (player.pos[0] > maxX)
    {
        player.pos[0] = minX;
    }

    if (player.pos[1] < minY)
    {
        player.pos[1] = maxY;
    }

    if (player.pos[1] > maxY)
    {
        player.pos[1] = minY;
    }
}

/***************************************
* Export DLL functions
***************************************/
#include "app_dll_export.cpp"

#endif