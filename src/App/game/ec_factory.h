#pragma once

#include "game.h"

struct Ent_UpdateHeader
{
    EntId entId;
    u32 componentBits;
};

#if 0
u32 EC_AIController_ApplyState(GameState* gs, Ent* ent, u8* ptr)
{
    u8* origin = ptr;
    EC_AIController update;
    COM_COPY_STRUCT(ptr, &update, EC_AIController);

    EC_AIController ai = EC_FindAIController(gs, entId);
    
    return 0;
}
#endif

u32 EC_Transform_ApplyState(GameState* gs, Ent* ent, u8* ptr)
{
    return 0;
}

u32 EC_Renderer_ApplyState(GameState* gs, Ent* ent, u8* ptr)
{
    return 0;
}

#define EC_UPDATE_CALL(gameState, flag, func) \
{ \
    if (messageComponentBits & flag##) \
    { \
        stream += func##(gameState, ent, stream); \
    } \
} 

u32 Ent_ApplyState(GameState* gs, Ent* ent, u32 messageComponentBits, u8* stream)
{
    u8* origin = stream;

    EC_UPDATE_CALL(gs, EC_FLAG_TRANSFORM, EC_Transform_ApplyState)
    EC_UPDATE_CALL(gs, EC_FLAG_RENDERER, EC_Renderer_ApplyState)
    return (stream - origin);
}

u32 Test_WriteTestEntityBuffer(GameState* gs, u8* stream, i32 streamCapacity)
{
    u8* origin = stream;
    Ent_UpdateHeader h = {};
    h.entId = Ent_ReserveFreeEntity(&gs->entList);
    h.componentBits |= EC_FLAG_TRANSFORM;
    h.componentBits |= EC_FLAG_RENDERER;

    Transform t = {};
    Transform_SetToIdentity(&t);
    t.pos.y += 0.5f;
    stream += COM_COPY_STRUCT(&t, stream, Transform);

    EC_RendererState r = {};
    COM_CopyStringLimited("cube", r.meshName, EC_RENDERER_STRING_LENGTH);
    COM_CopyStringLimited(("textures\\W33_5.bmp"), r.textureName, EC_RENDERER_STRING_LENGTH);

    stream += COM_COPY_STRUCT(&r, stream, EC_RendererState);

    return (stream - origin);
}
