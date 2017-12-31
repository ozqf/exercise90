#pragma once

#include "../Shared/shared.h"
#include "app_interface.h"
#include "platform_interface.h"

#include <stdio.h>

/*****************************************************
Empty, default App implementation
*****************************************************/

PlatformInterface platformStub;

struct AppStubState
{
    i32 drawIndex = 0;
    i32 numDrawItems = 3;
    i32 frameCounter = 0;
    i32 frameMax = 0;
    BlitItem blitItem;
};

AppStubState appStubState;

void AppInit_Stub()
{
    printf("Init App Stub\n");
    appStubState.drawIndex = 0;
    appStubState.numDrawItems = 3;
    appStubState.frameCounter = 0;
    appStubState.frameMax = 50;

    appStubState.blitItem.type = BLIT_ITEM_BLOCK;
    appStubState.blitItem.content.block.centre[0] = 10;
    appStubState.blitItem.content.block.centre[1] = 4;
    appStubState.blitItem.content.block.halfWidth = 4;
    appStubState.blitItem.content.block.halfHeight = 4;
    appStubState.blitItem.content.block.red = 0xFF;
    appStubState.blitItem.content.block.green = 0x00;
    appStubState.blitItem.content.block.blue = 0x00;
    
    // BlitBlock* block = &appStubState.blitItem.content.block;
    // block->centre[0] = 4;
    // block->centre[1] = 4;
    // block->halfWidth = 4;
    // block->halfWidth = 4;
    // block->red = 0xFF;
    // block->green = 0x00;
    // block->blue = 0x00;
}
void AppShutdown_Stub() { printf("Shutdown App Stub\n"); }
void AppUpdate_Stub(GameTime* time, InputTick* tick)
{
    platformStub.PlatformClearScreen();
    AppStubState* s = &appStubState;
    switch (s->drawIndex)
    {
        case 1:
            s->blitItem.content.block.red = 0x00;
            s->blitItem.content.block.green = 0xFF;
            s->blitItem.content.block.blue = 0x00;
        break;  
        case 2:
            s->blitItem.content.block.red = 0x00;
            s->blitItem.content.block.green = 0x00;
            s->blitItem.content.block.blue = 0xFF;
        break;  
        default:
            s->blitItem.content.block.red = 0xFF;
            s->blitItem.content.block.green = 0x00;
            s->blitItem.content.block.blue = 0x00;
        break;
    }
    platformStub.PlatformRenderBlitItems(&s->blitItem, 1);
}

// TODO: This doesn't work... lawl
void AppFixedUpdate_Stub(GameTime* time, InputTick* tick)
{
    AppStubState* s = &appStubState;
    if (s->frameCounter <= 0)
    {
        s->frameCounter = s->frameMax;
        s->drawIndex++;
        if (s->drawIndex >= s->numDrawItems)
        {
            s->drawIndex = 0;
        }
        switch (s->drawIndex)
        {
            case 1:
            printf("Green\n");
            break;

            case 2:
            printf("Blue\n");
            break;

            default:
            printf("Red\n");
            break;
        }
    }
    s->frameCounter--;
}

// Fill out an app stub
AppInterface GetAppInterfaceStub(PlatformInterface platInterface)
{
    printf("Link to App Stub\n");
    platformStub = platInterface;

    AppInterface app = { };
    app.isvalid = true;
    app.AppInit = AppInit_Stub;
    app.AppShutdown = AppShutdown_Stub;
    app.AppUpdate = AppUpdate_Stub;
    app.AppFixedUpdate = AppFixedUpdate_Stub;
    return app;
}
