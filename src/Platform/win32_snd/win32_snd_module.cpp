#pragma once

#include "../../../src/common/com_defines.h"
#include "win32_snd_interface.h"

#include "../../../lib/fmod/fmod_studio.hpp"
#include "../../../lib/fmod/fmod.hpp"

internal FMOD::Sound* gsnd_soundHandle;
internal FMOD::Channel* gsnd_channel;

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

#if 1 // Low level API only
u8 Snd_Init()
{
    FMOD_RESULT result;
    FMOD::System* system = NULL;

    result = FMOD::System_Create(&system); // Create the Studio System object.
    if (result != FMOD_OK)
    {
        return 0;
    }

    // Initialize FMOD Low Level
    result = system->init(512, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        //printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        return 0;
    }

    // test example, commented out until I've made a decision on asset storage/version control
    //result = system->createSound("Frenzy_Beam_Loop.wav", FMOD_DEFAULT, NULL, &gsnd_soundHandle);
    //result = system->playSound(gsnd_soundHandle, NULL, false, &gsnd_channel);

    return 1;
}
#endif

u8 Snd_Shutdown()
{
    return 1;
}

#include "win32_snd_dll_Export.h"
