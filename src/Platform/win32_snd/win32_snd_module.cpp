#pragma once

#include "../../../src/common/com_defines.h"
#include "win32_snd_interface.h"

#include "../../../lib/fmod/fmod_studio.hpp"
#include "../../../lib/fmod/fmod.hpp"

#include <stdio.h>

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

FMOD::System* sys = NULL;

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

u8 Snd_LoadSound(u8* data, i32 numBytes)
{
    printf("SOUND Creating sound, %d bytes\n", numBytes);
    // test example, commented out until I've made a decision on asset storage/version control
    FMOD_CREATESOUNDEXINFO info = {};
    info.filebuffersize = numBytes;
    FMOD_RESULT result = sys->createSound((const char*)data, FMOD_OPENMEMORY, &info, &gsnd_soundHandle);
    result = sys->playSound(gsnd_soundHandle, NULL, false, &gsnd_channel);
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
    FMOD_CREATESOUNDEXINFO info = {};
    info.fileBufferSize = 
    result = sys->createSound("Frenzy_Beam_Loop.wav", FMOD_OPENMEMORY, NULL, &gsnd_soundHandle);
    result = sys->playSound(gsnd_soundHandle, NULL, false, &gsnd_channel);
#endif
    return 1;
}
#endif

u8 Snd_Shutdown()
{
    return 1;
}

#include "win32_snd_dll_Export.h"
