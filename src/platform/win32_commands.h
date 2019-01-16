#pragma once

#include "win32_module.cpp"

global_variable i32 g_nextTextCommandIndex = 0;
global_variable ZTextCommandHandler g_textCommands[64];

#define TEXT_COMMAND_HANDLER(funcName) u8 TextExec_##funcName##(char* text, char** tokens, i32 numTokens)

u8 TextExec_ConsoleHelp(char* text, char** tokens, i32 numTokens)
{
    printf("--- Command List ---\n");
    printf("  QUIT or EXIT - shutdown\n");
    printf("  VERSION - show version info\n");
    printf("  MANIFEST - List data files entries\n");
    printf("  RESTART APP/RENDERER/SOUND/GAME - Reload subsystem\n");
    printf("  KEYCODES - toggle printing keycodes in debug input\n");
    printf("  LAPTOP - Toggle mode to give up cpu time each frame\n");
    return 0;
}

u8 TextExec_Quit(char* text, char** tokens, i32 numTokens)
{
    Win32_Shutdown();
    return 1;
}

TEXT_COMMAND_HANDLER(Version)
{
    printf("--- VERSIONS ---\n");
    printf("PLATFORM Built %s: %s\n", __DATE__, __TIME__);
    return 0;
}

TEXT_COMMAND_HANDLER(Mark)
{
    printf("/////////////////////////////////////////\n");
    printf("// Kernel frame: %d\n",
        g_time.frameNumber);
    printf("/////////////////////////////////////////\n");
    return 1;
}

TEXT_COMMAND_HANDLER(Manifest)
{
    Win32_DebugPrintDataManifest();
    return 1;
}

TEXT_COMMAND_HANDLER(Restart)
{
    if (numTokens == 2)
    {
        if (COM_CompareStrings(tokens[1], "APP") == 0)
        {
            printf("  Restarting app\n");
            g_appLink.timestamp = {};
            return 1;
        }
        else if (COM_CompareStrings(tokens[1], "RENDERER") == 0)
        {
            printf("  Restarting renderer\n");
            g_rendererLink.timestamp = {};
            return 1;
        }
        else if (COM_CompareStrings(tokens[1], "SOUND") == 0)
        {
            printf("  Restarting sound\n");
            g_soundLink.timestamp = {};
            return 1;
        }
    }
    return 1;
}

TEXT_COMMAND_HANDLER(Hello)
{
    printf("HI.\n");
    return 1;
}

TEXT_COMMAND_HANDLER(Time)
{
    SYSTEMTIME t;
    GetSystemTime(&t);
    printf("  TIME: %d/%d/%d - %d:%d:%d\n",
        t.wYear,
        t.wMonth,
        t.wDay,
        t.wHour,
        t.wMinute,
        t.wSecond
    );
    return 1;
}

TEXT_COMMAND_HANDLER(StepMode)
{
    g_singleFrameStepMode = !g_singleFrameStepMode;
    printf("PLATFORM Single frame mode: %d\n", g_singleFrameStepMode);
    return 1;
}

TEXT_COMMAND_HANDLER(Step)
{
    g_singleFrameRun = 1;
    printf("PLATFORM Step frame\n");
    return 1;
}

TEXT_COMMAND_HANDLER(Break)
{
    DebugBreak();
    return 1;
}

TEXT_COMMAND_HANDLER(KeyCodes)
{
    g_debugPrintKeycodes = !g_debugPrintKeycodes;
    printf("PLATFORM: Print keycodes: %d\n", g_debugPrintKeycodes);
    return 1;
}

TEXT_COMMAND_HANDLER(Laptop)
{
    g_lowPowerMode = !g_lowPowerMode;
    printf("PLATFORM: Low power mode: %d\n", g_lowPowerMode);
    return 1;
}

void Win32_AddTextCommand(char* name, i32 minTokens, i32 maxTokens, ZParseTextCommand func)
{
    u8 error = COM_ValidateTextCommandHandler(name, minTokens, maxTokens, func);
    if (error != 0)
    {
        printf("Error creating command %s: %d\n", name, error);
        return;
    }

    ZTextCommandHandler* h = &g_textCommands[g_nextTextCommandIndex];
    g_nextTextCommandIndex++;
    
    COM_CopyStringLimited(name, h->name, 32);
    h->minTokens = minTokens;
    h->maxTokens = maxTokens;
    h->func = func;
}

void Win32_BuildTextCommandList()
{
    Win32_AddTextCommand("HELP", 1, 1, TextExec_ConsoleHelp);
    Win32_AddTextCommand("QUIT", 1, 1, TextExec_Quit);
    Win32_AddTextCommand("EXIT", 1, 1, TextExec_Quit);
    Win32_AddTextCommand("VERSION", 1, 1, TextExec_Version);
    Win32_AddTextCommand("MANIFEST", 1, 1, TextExec_Manifest);
    Win32_AddTextCommand("TIME", 1, 1, TextExec_Time);
    Win32_AddTextCommand("RESTART", 2, 2, TextExec_Restart);
    Win32_AddTextCommand("HELLO", 1, 1, TextExec_Hello);
    Win32_AddTextCommand("STEPMODE", 1, 1, TextExec_StepMode);
    Win32_AddTextCommand("Z", 1, 1, TextExec_StepMode);
    Win32_AddTextCommand("STEP", 1, 1, TextExec_Step);
    Win32_AddTextCommand("BREAK", 1, 1, TextExec_Break);
    Win32_AddTextCommand("KEYCODES", 1, 1, TextExec_KeyCodes);
    Win32_AddTextCommand("LAPTOP", 1, 1, TextExec_Laptop);
    Win32_AddTextCommand("MARK", 1, 1, TextExec_Mark);
}

/**********************************************************************
 * EXECUTE TEXT COMMAND
 *********************************************************************/
u8 Win32_ExecTextCommand(char* str, char** tokens, i32 numTokens)
{
    i32 l = g_nextTextCommandIndex;
    for (i32 i = 0; i < l; ++i)
    {
        ZTextCommandHandler* h = &g_textCommands[i];
        #if 1
        if (!COM_CompareStrings(tokens[0], h->name))
        {
            //printf("Matched cmd %s\n", h->name);
            if (numTokens < h->minTokens || numTokens > h->maxTokens)
            {
                printf("Invalid token count. Got %d. Supports %d to %d\n",
                    numTokens, h->minTokens, h->maxTokens);
                return 1;
            }
            return h->func(str, tokens, numTokens);
        }
        #endif
    }
    return 0;
}

void Win32_EnqueueTextCommand(char* command)
{
    i32 length = COM_StrLen(command) + 1;
    i32 capacity = TEXT_COMMAND_BUFFER_SIZE - g_textCommandBufferPosition;
    if (capacity <= length)
    {
        Win32_Error("Text command buffer overrun", "Buffer overrun");
    }
    g_textCommandBufferPosition = COM_EnqueueTextCommand(
        command,
        g_textCommandBuffer,
        g_textCommandBufferPosition,
        TEXT_COMMAND_BUFFER_SIZE
    );
}

#define TEXT_CMD_CONTINUE 0
#define TEXT_CMD_WAIT 1

i32 Win32_ExecuteTextCommand(char* command)
{
    // Avoid executing on the original string
    char commandCopy[256];
    // null split version of command copy
    char tokensBuffer[256];
    // pointers to start of each token in tokensBuffer
    char *tokens[32];
    AssertAlways(COM_StrLen(command) < 256);
    COM_CopyString(command, commandCopy);

    i32 tokensCount = COM_ReadTokens(commandCopy, tokensBuffer, tokens);
    if (tokensCount == 0) { return TEXT_CMD_CONTINUE; }
	
	if (!COM_CompareStrings(tokens[0], "WAIT"))
	{
		return TEXT_CMD_WAIT;
	}
	
    u8 handled;
    handled = Win32_ExecTextCommand(commandCopy, tokens, tokensCount);
    if (handled) { return TEXT_CMD_CONTINUE; }
    
    if (g_app.isValid)
    {
        handled = g_app.AppParseCommandString(commandCopy, tokens, tokensCount);
        if (handled) { return TEXT_CMD_CONTINUE; }
    }

    handled = g_sound.Snd_ParseCommandString(g_textCommandBuffer, tokens, tokensCount);
    if (handled) { return TEXT_CMD_CONTINUE; }

    // Version is allowed to cascade down so each subsystem prints it's version
    if (COM_CompareStrings(tokens[0], "VERSION") == 0) { return TEXT_CMD_CONTINUE; }

    printf(" Unknown command %s\n", g_textCommandBuffer);
	return TEXT_CMD_CONTINUE;
}

void Win32_ExecuteTextCommands()
{
    char executeBuffer[256];
    i32 written = 0;
    i32 position = 0;

    i32 executeCount = 0;

    for (;;)
    {
        written = COM_DequeueTextCommand(g_textCommandBuffer, executeBuffer, position, 256);
        position += written;
        if (written <= 1)
        {
            break;
        }
        printf("EXEC %s\n", executeBuffer);
        i32 result = Win32_ExecuteTextCommand(executeBuffer);
		if (result == TEXT_CMD_WAIT)
		{
			// Continue on next frame
            i32 remaining = COM_StrLen(&g_textCommandBuffer[position]);
			printf("PLATFORM Waiting at text buffer pos %d\n", g_textCommandBufferPosition);
            //g_textCommandBufferPosition = position;
            printf("  REMAINING: %d chars:\n%s\n", remaining, &g_textCommandBuffer[position]);
            u8* source = (u8*)&g_textCommandBuffer[position];
            u8* dest = (u8*)&g_textCommandBuffer[0];
            printf("Copying from %d to %d\n", (u32)source, (u32)dest);
            COM_CopyMemory(source, dest, remaining);
            //COM_COPY(&g_textCommandBuffer[position], &g_textCommandBuffer[0], remaining);
            break;
			//return;
		}
        executeCount++;
    }
    // if (executeCount > 0)
    // {
    //     printf("PLATFORM Executed %d text commands\n", executeCount);
    // }

    // Clear buffer entirely
    // TODO: Allow commands to wait for next frame...?
    g_textCommandBufferPosition = 0;
}

/**********************************************************************
 * PARSE COMMAND LINE
 *********************************************************************/
 void Win32_ExecuteCommandLine(i32 argc, char** argv)
 {
	 char buf[256];
	 COM_ZeroMemory((u8*)buf, 256);
	 i32 written = COM_ConcatonateTokens(buf, 256, argv, argc, 0);
	 printf("PLATFORM exec launch string %s\n", buf);
	 Win32_EnqueueTextCommand(buf);
 }
 