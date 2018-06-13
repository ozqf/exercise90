#ifndef WIN32_SND_INTERNAL_H
#define WIN32_SND_INTERNAL_H

// A playable sound sample loaded by FMOD
struct ZSound
{
	i32 id;
	char name[64];
	FMOD::Sound* handle;    // if handle is null, this sound is unassigned
};

// An entity in the sound 'scene'
struct ZSoundSource
{
	i32 id;
	i32 soundId;            // id of -1 == unused
	f32 pos[3];
	f32 tick;
};

#endif
