#pragma once

#include "../../../lib/fmod/fmod_studio.hpp"
#include "../../../lib/fmod/fmod.hpp"

#include <stdio.h>

#include "../../../src/common/com_module.h"
#include "../../interface/sound_interface.h"
#include "win32_snd_interface.h"
#include "win32_snd_internal.h"

/*
Sound notes:

-- Types of sounds --
> Spatial sounds: Anything in the 'world' that has a position and direction
    eg Gunfire, enemies, explosions.
> Ambient: Sounds which don't necessarily have direction but still vary
    by volume depending on position. eg wind, flowing water etc.
> Non-spatial one-off:
    eg UI, own character sounds, item pickups etc.
> Non-spatial, looping, constant volume:
    eg Music

*/

/////////////////////////////////////////////////////////////////////
// External functions
/////////////////////////////////////////////////////////////////////
i32 Snd_LoadSound(char* name64, u8* data, i32 numBytes);
u8 Snd_Play2(ZSoundEvent* ev);
u8 Snd_Init();
u8 Snd_ParseCommandString(char* str, char** tokens, i32 numTokens);

/////////////////////////////////////////////////////////////////////
// Internal functions
u8 Snd_Shutdown();
/////////////////////////////////////////////////////////////////////
u8 SndClearWorldScene();
u8 SndClearUIScene();
u8 SndClearBGM();

/////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////

// https://www.fmod.com/docs/api/content/generated/FMOD_MODE.html
// flag to tell fmod to read name_or_data params as data, not file names
// In open memory mode fmod will duplicate the data so it can be freed
// once loaded
// bit field FMOD_OPENMEMORY
// this flag does the same as the above but does NOT copy data,
// so data must be manually handled. requires Sound::release to free
// bit field FMOD_OPENMEMORY_POINT

internal FMOD::Sound* gsnd_soundHandle;
internal FMOD::Channel* gsnd_channel;

ZSound g_sounds[128];
i32 g_nextSoundId = 0;
ZSoundSource g_sources[128];

FMOD::Channel* g_channels[32];

FMOD::System* sys = NULL;

internal u8 g_testSoundLoaded = 0;

//internal ZSound g_sounds[128];

u8 Snd_Play2(ZSoundEvent* ev)
{
    //gsnd_channel.set3DAttributes()
    return 1;
}

#if 0 // FMOD Studio - broken atm
u8 Snd_Init()
{
    FMOD_RESULT result;
    FMOD::Studio::System* system = NULL;

    result = FMOD::Studio::System::create(&system); // Create the Studio System object.
    if (result != FMOD_OK)
    {
        return 1;
    }

    // Initialize FMOD Studio, which will also initialize FMOD Low Level
    result = system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        //printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        return 1;
    }

    result = FMOD::Studio::System::createSound("Frenzy_Beam_Loop.wav", FMOD_DEFAULT, NULL, &g_soundHandle);

    return 0;
}
#endif

// Returns id of new sound
i32 Snd_LoadSound(char* name64, u8* data, i32 numBytes)
{
    printf("SOUND Creating sound, %d bytes from %d\n", numBytes, (u32)data);
    // test example, commented out until I've made a decision on asset storage/version control
    FMOD_CREATESOUNDEXINFO info = {};
    info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    info.length = numBytes;
    FMOD_RESULT result = sys->createSound((const char*)data, FMOD_OPENMEMORY, &info, &gsnd_soundHandle);
    printf(" create result: %d\n", result);
    if (result == FMOD_OK)
    {
        g_testSoundLoaded = 1;
    }
    return 1;
}

#if 1 // Low level API only
u8 Snd_Init()
{
    printf("SOUND Init\n");
    FMOD_RESULT result;
    //FMOD::System* system = NULL;

    result = FMOD::System_Create(&sys); // Create the Studio System object.
    if (result != FMOD_OK)
    {
        return 0;
    }

    // Initialize FMOD Low Level
    result = sys->init(512, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        //printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        return 0;
    }
#if 0
    // test example, commented out until I've made a decision on asset storage/version control
    result = sys->createSound("Frenzy_Beam_Loop.wav", 0, NULL, &gsnd_soundHandle);
    result = sys->playSound(gsnd_soundHandle, NULL, false, &gsnd_channel);
#endif
    return 1;
}
#endif

u8 Snd_Shutdown()
{
    printf("SOUND Shutting down\n");
    FMOD_RESULT result;
    if (g_testSoundLoaded == 1)
    {
        result = gsnd_soundHandle->release();
        printf("SND release result: %d\n", result);
    }
    return 1;
}

u8 Snd_ParseCommandString(char* str, char** tokens, i32 numTokens)
{
    if (COM_CompareStrings(tokens[0], "SND") == 0)
    {
        if (numTokens == 2 && COM_CompareStrings(tokens[1],"TEST") == 0)
        {
            if (g_testSoundLoaded == 0)
            {
                printf("Cannot play sound test - no sounds loaded");
                return 1;
            }
            FMOD_RESULT result = sys->playSound(gsnd_soundHandle, NULL, false, &gsnd_channel);
            printf("  play result: %d\n", result);
        }
        return 1;
    }
    if (COM_CompareStrings(tokens[0], "VERSION") == 0)
	{
		printf("SOUND Built %s: %s\n", __DATE__, __TIME__);
		return 0;
	}
    return 0;
}

#include "win32_snd_dll_Export.h"
