#pragma once

//#include "../common/com_module.h"


enum ZSoundEventType
{
    PlaySound,
    MoveSound
};

union ZSoundEventUnion
{
    struct PlaySound
    {
        u8 spatial;
	    f32 pos[3];
    };

    struct MoveSound
    {
        f32 pos[3];
    };
};

// An event that kicks off a sound source
struct ZSoundEvent
{
	i32 soundId;
    ZSoundEventType type;
    ZSoundEventUnion data;
};

void SND_SetPlaySoundEvent(ZSoundEvent* ev, i32 soundId, u8 spatial)
{
    ev->type = PlaySound;
}

void SND_SetMoveSoundEvent(ZSoundEvent* ev)
{
    
}
