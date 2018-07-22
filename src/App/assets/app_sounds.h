#pragma once


/*
Manage sound assets
*/

#include "app_module.cpp"

///////////////////////////////////////
// Interface
///////////////////////////////////////

// List all registered sounds
// (these sounds are in app memory, but they might not be bound to the sound system)
void AppListSounds();

// Search the list of currently registered sounds for it's id.
// returns -1 if the sound was not found
i32 AppGetSoundIndexByName(char* soundName);

// Add an app record for the given sound, returning the sounds assigned id
i32 AppRegisterSound(SoundHeader* header);

// Upload sound file to sound system. Only bound sounds can be played
// attempting to bind to an id that has been previously bound will
// cause a halt
void AppBindSound(SoundHeader* header);

// Unbind the given sound, freeing the given id
void AppUnbindSound(i32 id);

// Load, Register and bind a sound file, returning the sounds id
i32 AppLoadAndBindSound(char* filePath);

// Upload all registered sounds to the sound system
// Must be called if the sounds system has been reloaded!
void AppBindAllSounds();

// The platform has reloaded the sound interface.
// Rebind sounds if necessary
void AppHandleSoundSystemReload();


///////////////////////////////////////
// Implementation
///////////////////////////////////////

void AppListSounds()
{
    i32 l = g_soundHandles.numSounds;
    printf("--- APP SOUNDS (%d) ---\n", l);
    for (i32 i = 0; i < l; ++i)
    {
        SoundHeader* s = &g_soundHandles.sounds[i];
        printf("%d: \"%s\"", i, s->name);
    }
}
