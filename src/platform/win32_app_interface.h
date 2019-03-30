#pragma once
/*
Functions exported to platform DLL
Anything with platform_ is a function to export
*/

#include "win32_system_include.h"
#include "win32_fileIO.h"

#include "../interface/sound_interface.h"
//#include "win32_gl_loading.h"

/****************************************************************
ALLOC MAIN MEMORY
Returns true if alloc succeeded
****************************************************************/
u8 Win32_Alloc(MemoryBlock *mem, u32 bytesToAllocate)
{
    mem->size = bytesToAllocate;
    mem->ptrMemory = VirtualAlloc(0,mem->size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    return (mem->ptrMemory != NULL);
}

void Win32_Free(MemoryBlock *mem)
{
    if (mem)
    {
        // Reset to 0
        char *cursor = (char *)mem->ptrMemory;
        for (int i = 0; i < mem->size; ++i)
        {
            *cursor = 0;
            cursor++;
        }
        VirtualFree(mem->ptrMemory, 0, MEM_RELEASE);
        mem->ptrMemory = 0;
    }
}

/**********************************************************************
 * PLATFORM INTERFACE FUNCTIONS
 *********************************************************************/
void Platform_R_DrawScene(RenderScene* scene)
{
    if (g_rendererLink.moduleState == 1)
    {
        g_renderer.R_RenderScene(scene, &g_time);
    }
}

void Platform_R_SubmitCommands(RenderCommand* commands, i32 numCommands)
{
    if (g_rendererLink.moduleState == 1)
    {
        g_renderer.R_Execute(commands, numCommands, &g_time);
    }
}

void Platform_LoadTexture(Heap* heap, BlockRef* result, char* path)
{
    //Win32_ReadBMPToHeap(heap, result, path);
	Platform_LoadFileIntoHeap(heap, result, path, true);
}

Texture2DHeader* LoadTextureB(Com_AllocateTexture callback, char* path)
{
    return Win32_LoadTextureB(callback, path);
}

void Platform_BindTexture(void* rgbaPixels, u32 width, u32 height, u32 textureIndex)
{
    if (g_rendererLink.moduleState == 1)
    {
        g_renderer.R_BindTexture(rgbaPixels, width, height, textureIndex);
    }
}

void Platform_SetDebugInputTextureIndex(i32 i)
{
    Win32_SetDebugInputTextureIndex(i);
}

// Copy the base name into the given buffer
i32 Platform_GetBaseDirectoryName(char* buffer, i32 bufferSize)
{
    return COM_CopyStringLimited(g_gameName, buffer, bufferSize);
}

void Platform_GetDateTime(DateTime* data)
{
    if (data == NULL) { return; }
    SYSTEMTIME time = {};
    GetSystemTime(&time);
    data->year = time.wYear;
    data->month = time.wMonth;
    data->dayOfTheMonth = time.wDay;
    data->dayOfTheWeek = time.wDayOfWeek;
    data->hour = time.wHour;
    data->minute = time.wMinute;
    data->second = time.wSecond;
}

void Win32_Log(char* msg)
{
    if (g_logFile)
    {
        fprintf(g_logFile, "%s", msg);
    }
}

void Win32_Print(char* msg)
{
    if (g_logFile)
    {
        fprintf(g_logFile, "%s", msg);
    }
    printf("%s", msg);
}

internal i32 Win32_WriteAllTextToFile(char* fileName, char* contents)
{
    FILE* f;
    i32 err = fopen_s(&f, fileName, "w");
    if (err)
    {
        printf("Write all text Error code %d\n", err);
        //Win32_Error(fileName, "Error writing text file");
        return COM_ERROR_UNKNOWN;
    }

    fprintf(f, "%s", contents);

    fclose(f);
    return COM_ERROR_NONE;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Sound
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void Platform_LoadSound(u8* data, i32 numBytes)
{
    if (g_soundLink.moduleState == 1)
    {
        g_sound.Snd_LoadSound("foo", data, numBytes);
    }
}

u8 Snd_Play(ZSoundEvent* ev)
{
    if (g_soundLink.moduleState == 1)
    {
        return g_sound.Snd_Play2(ev);
    }
    return 0;
}
