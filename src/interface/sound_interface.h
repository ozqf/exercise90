#pragma once

//#include "../common/common.h"


enum ZSoundEventType
{
    zPlaySound,
    zMoveSound
};

union ZSoundEventUnion
{
    struct ZPlaySound
    {
        u8 spatial;
	    f32 pos[3];
    };

    struct ZMoveSound
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
    ev->type = zPlaySound;
}

void SND_SetMoveSoundEvent(ZSoundEvent* ev)
{
    
}
