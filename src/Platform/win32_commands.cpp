#pragma once

#include "win32_module.cpp"


/**********************************************************************
 * EXECUTE TEXT COMMAND
 *********************************************************************/
u8 Win32_ExecTextCommand(char *str, char **tokens, i32 numTokens)
{
    if (COM_CompareStrings(tokens[0], "QUIT") == 0 || COM_CompareStrings(tokens[0], "EXIT") == 0)
    {
        Win32_Shutdown();
        return 1;
    }
    else if (COM_CompareStrings(tokens[0], "HELP") == 0)
    {
        printf("--- Command List ---\n");
        printf("  QUIT or EXIT - shutdown\n");
        printf("  VERSION - show version info\n");
        printf("  MANIFEST - List data files entries\n");
        printf("  RESTART APP/RENDERER/SOUND/GAME - Reload subsystem\n");
        return 0;
    }
    else if (COM_CompareStrings(tokens[0], "VERSION") == 0)
    {
        printf("--- VERSIONS ---\n");
        printf("PLATFORM Built %s: %s\n", __DATE__, __TIME__);
        return 0;
    }
    else if (COM_CompareStrings(tokens[0], "MANIFEST") == 0)
    {
        Win32_DebugPrintDataManifest();
        return 1;
    }
    else if (COM_CompareStrings(tokens[0], "RESTART") == 0)
    {
        if (numTokens != 2)
        {
            printf("  Must specify: APP, RENDERER or SOUND\n");
            return 1;
        }

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
    else if (COM_CompareStrings(tokens[0], "TIME") == 0)
    {
        //printf("HI.");
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
    else if (COM_CompareStrings(tokens[0], "HELLO") == 0)
    {
        printf("HI.\n");
        return 1;
    }
    else if (COM_CompareStrings(tokens[0], "Z") == 0 || COM_CompareStrings(tokens[0], "STEPMODE") == 0)
    {
        g_singleFrameStepMode = !g_singleFrameStepMode;
        printf("PLATFORM Single frame mode: %d\n", g_singleFrameStepMode);
        return 1;
    }
    else if (COM_CompareStrings(tokens[0], "STEP") == 0)
    {
        g_singleFrameRun = 1;
        printf("PLATFORM Step frame\n");
        return 1;
    }
    else if (COM_CompareStrings(tokens[0], "BREAK") == 0)
    {
        DebugBreak();
        return 1;
    }
    return 0;
}

#if 0
void Win32_ReadTextCommand(char *str, i32 firstChar, i32 length)
{
    COM_ZeroMemory((u8 *)g_textCommandBuffer, TEXT_COMMAND_BUFFER_SIZE);
    COM_COPY(str, g_textCommandBuffer, length);
    //printf("EXEC \"%s\" (%d chars)\n", g_textCommandBuffer, length);

    // Avoid executing on the original string
    char commandCopy[256];
    char tokensBuffer[256];
    char *tokens[32];

    COM_CopyString(g_textCommandBuffer, commandCopy);
    i32 tokensCount = COM_ReadTokens(g_textCommandBuffer, tokensBuffer, tokens);
    if (tokensCount == 0)
    {
        return;
    }
    #if 0
    printf("PRINT TOKENS (%d)\n", tokensCount);
    for (int i = 0; i < tokensCount; ++i)
    {
        //char* str2 = (char*)(tokensBuffer + tokens[i]);
        printf("%s\n", tokens[i]);
    }
    #endif

    u8 handled;
    handled = Win32_ExecTestCommand(g_textCommandBuffer, tokens, tokensCount);
    if (handled)
    {
        return;
    }

    if (g_app.isValid)
    {
        handled = g_app.AppParseCommandString(g_textCommandBuffer, tokens, tokensCount);
    }
    if (handled)
    {
        return;
    }

    handled = g_sound.Snd_ParseCommandString(g_textCommandBuffer, tokens, tokensCount);
    if (handled)
    {
        return;
    }

    // Version is allowed to cascade down so each subsystem prints it's version
    if (COM_CompareStrings(tokens[0], "VERSION") == 0)
    {
        return;
    }

    printf(" Unknown command %s\n", g_textCommandBuffer);
    return;
}

void Win32_ParseTextCommand(char *str, i32 firstChar, i32 length)
{
    // Gate execution
    static i32 isExecuting = 0;
    if (isExecuting)
    {
        printf("PLATFORM: attempted to parse \"%s\" but already executing!\n", str);
        return;
    }
    isExecuting = 1;

    Win32_ReadTextCommand(str, firstChar, length);
    isExecuting = 0;
}
#endif

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

void Win32_ExecuteTextCommand(char* command)
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
    if (tokensCount == 0) { return; }

    u8 handled;
    handled = Win32_ExecTextCommand(commandCopy, tokens, tokensCount);
    if (handled) { return; }
    
    if (g_app.isValid)
    {
        handled = g_app.AppParseCommandString(commandCopy, tokens, tokensCount);
        if (handled) { return; }
    }

    handled = g_sound.Snd_ParseCommandString(g_textCommandBuffer, tokens, tokensCount);
    if (handled) { return; }

    // Version is allowed to cascade down so each subsystem prints it's version
    if (COM_CompareStrings(tokens[0], "VERSION") == 0) { return; }

    printf(" Unknown command %s\n", g_textCommandBuffer);
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
        Win32_ExecuteTextCommand(executeBuffer);
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
