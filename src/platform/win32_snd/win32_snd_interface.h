#pragma once

#include "../../common/common.h"

struct SoundInterface
{
	///////////////////////////////////////
	// DLL life time
	///////////////////////////////////////
    u8 (*Snd_Init)();
    u8 (*Snd_Shutdown)();
	
	///////////////////////////////////////
	// App lifetime
	///////////////////////////////////////
	
	// Release ALL sounds and sound memory.
	//u8 (*Snd_UnloadAll)();
	
	// Add a reference to a sound
    i32 (*Snd_LoadSound)(char* name64, u8* data, i32 numBytes);
	
	///////////////////////////////////////
	// Actually make it do stuff
	///////////////////////////////////////
	u8 (*Snd_ParseCommandString)(char* str, char** tokens, i32 numTokens);
	u8 (*Snd_Play2)(ZSoundEvent* ev);
};

typedef SoundInterface (Func_LinkToSound)();
