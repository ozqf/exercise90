#ifndef WIN32_DEBUG_CPP
#define WIN32_DEBUG_CPP

#define WIN32_DEBUG_ENABLED 0

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "win32_system_include.h"

/****************************************************************
Create a command window for debug output
****************************************************************/

void InitDebug()
{
#if WIN32_DEBUG_ENABLED

    /**
     * https://justcheckingonall.wordpress.com/2008/08/29/console-window-win32-app/
     * Spawns cmd but window is empty and printf doesn't work
     */

    AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    //*stdout = *hf_out;
    // minus stdout define:
    (*__acrt_iob_func(1)) = *hf_out;

    FILE * current = __acrt_iob_func(1);
    *current = *hf_out;

    //*__acrt_iob_func = *hf_out;
	// #define stdout (__acrt_iob_func(1))

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;

#endif
}

#endif