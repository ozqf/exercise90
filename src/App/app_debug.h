#pragma once

#include "app_module.cpp"
#if 0
void App_DebugPrintPlayers(GameState* gs)
{
    i32 l = gs->playerList.max;
    i32 count = 0;
    for (i32 i = 0; i < l; ++i)
    {
        Player* p = &gs->playerList.items[i];
        if (p->state == PLAYER_STATUS_FREE) { continue; }
        count++;
        printf("Plyr %d client %d state %d avatar %d/%d\n",
            p->playerId, p->sv_clientId, p->state,
            p->entId.iteration, p->entId.index
        );
    }
    printf(" %d players\n", count);
}
#endif

#if 1
void App_DebugPrintEntities(GameState* gs)
{
    i32 l = gs->entList.max;
    i32 count = 0;
    for (i32 i = 0; i < l; ++i)
    {
        Ent* e = &gs->entList.items[i];
        if (e->inUse == ENTITY_STATUS_FREE) { continue; }
        EC_Transform* ecT = EC_FindTransform(gs, e);
        if (ecT)
        {
            printf("%d/%d: Type %d status %d pos: %.2f %.2f %.2f\n",
            e->entId.iteration, e->entId.index,
            e->factoryType, e->inUse,
            ecT->t.pos.x, ecT->t.pos.y, ecT->t.pos.z
        );
        }
        else
        {
            printf("%d/%d: Type %d status %d\n",
            e->entId.iteration, e->entId.index,
            e->factoryType, e->inUse
        );
        }
        count++;
    }
    printf(" %d active entities\n", count);
}
#endif


char* g_bufferA_Name = "a";
char* g_bufferB_Name = "b";
char* g_bufferDemo_Name = "DEMO";
char* g_unknown = "Unknown";

char* App_GetBufferName(u8* bufferStartAddr)
{
    if (bufferStartAddr == g_appBufferA.ptrStart)
    {
        return g_bufferA_Name;
    }
    if (bufferStartAddr == g_appBufferB.ptrStart)
    {
        return g_bufferB_Name;
    }
    if (bufferStartAddr == g_replayReadBuffer.ptrStart)
    {
        return g_bufferDemo_Name;
    }
    return g_unknown;
}

void App_DebugListVariables(Var* vars, i32 numVars, i32 maxVars)
{
    printf("*** Variables - %d of %d max ***\n", numVars, maxVars);
    for (i32 i = 0; i < numVars; ++i)
    {
        printf("  %d: %s: %s\n", i, vars[i].name, vars[i].value);
    }
}

void AllocateDebugStrings(Heap *heap)
{
    // Buffer used for live stat output
    Heap_AllocString(&g_heap, &g_debugBuffer, 1024);
    char *writeStart = (char *)g_debugBuffer.ptrMemory;
    i32 charsWritten = sprintf_s(writeStart, g_debugBuffer.objectSize, "Debug Test string alloc from\nline %d\nIn file %s\nOn %s\n", __LINE__, __FILE__, __DATE__);
    sprintf_s(writeStart + charsWritten, g_debugBuffer.objectSize - charsWritten, "This text is appended to the previous line and\ncontains a 10 here: %d", 10);
    printf("APP Allocated debug string\n");
}

i32 AllocateTestStrings(Heap *heap)
{
    BlockRef ref = {};
    Heap_AllocString(&g_heap, &ref, 128);
    char *testStr1 = "This is a test string. It should get copied onto the heap";
    COM_CopyStringLimited(testStr1, (char *)ref.ptrMemory, ref.objectSize);

    ref = {};
    Heap_AllocString(&g_heap, &ref, 256);
    char *testStr2 = "Welcome to another test string. This one goes on and on and on and on and on and blooooody on. At most 256 though, no more, but possible less. Enough to require 256 bytes of capacity certainly. I mean, with more than that it would just break right? It'll go right off the end and just...";
    COM_CopyStringLimited(testStr2, (char *)ref.ptrMemory, ref.objectSize);
    return 1;
}



/////////////////////////////////////////////////////////////////////////////
// Write Debug String
/////////////////////////////////////////////////////////////////////////////

ZStringHeader App_WriteDebugString(GameState *gs, GameTime *time)
{
    //Vec3 pos, rot;

    ZStringHeader h;
    h.chars = gs->debugString;
    h.length = 0;
#if 1
    switch (gs->debugMode)
    {
        #if 0
        case GAME_DEBUG_MODE_CAMERA:
        {
            pos = gs->cameraTransform.pos;
            rot = gs->cameraTransform.rot;
            AngleVectors vectors = {};
            Calc_AnglesToAxesZYX(&rot, &vectors.left, &vectors.up, &vectors.forward);
            h.length = sprintf_s(gs->debugString, gs->debugStringCapacity,
                                 "Game.DLL:\nTimeDelta: %3.7f\n-- Camera --\nPos: %3.3f, %3.3f, %3.3f\nRot: %3.3f, %3.3f, %3.3f\nForward: %3.3f, %3.3f, %3.3f\nUp: %3.3f, %3.3f, %3.3f\nLeft: %3.3f, %3.3f, %3.3f\n",
                                 time->deltaTime,
                                 pos.x, pos.y, pos.z,
                                 rot.x, rot.y, rot.z,
                                 vectors.forward.x, vectors.forward.y, vectors.forward.z,
                                 vectors.up.x, vectors.up.y, vectors.up.z,
                                 vectors.left.x, vectors.left.y, vectors.left.z);
        }
        break;
        #endif

        case GAME_DEBUG_MODE_RECORDING:
        {
            i32 written = 0;
            if (g_replayMode == RecordingReplay)
            {
                written = sprintf_s(gs->debugString, gs->debugStringCapacity,
"Recorded %dKB\n", (g_replayHeader.frames.size / 1024)
                );
                h.length = written;
            }
            else if (g_replayMode == PlayingReplay)
            {
                i32 bytesRead = g_replayPtr - g_replayReadBuffer.ptrStart;
                f32 percentage = ((f32)bytesRead / (f32)g_replayReadBuffer.capacity) * 100.0f;
                written = sprintf_s(gs->debugString, gs->debugStringCapacity,
"Playing %d bytes of %d (%.2f%%)\n", bytesRead, g_replayReadBuffer.capacity, percentage);
            }
            else
            {
                printf("No debug for replay mode %d\n", g_replayMode);
            }
        } break;

        case GAME_DEBUG_MODE_TRANSFORM:
        {
            Ent *ent = Ent_GetEntityByTag(&gs->entList, 10);
            EC_Transform* ecT = EC_FindTransform(gs, ent);
            M3x3 rotation = {};
            Vec3 inputRot = {};
            Vec3 pos = {};
            Vec3 rot = {};
            Vec3 scale = {};
            if (ent == NULL || g_debugCameraOn)
            {
                rotation = g_debugCameraTransform.rotation;
                inputRot = g_debugCameraDegrees;
                scale = g_debugCameraTransform.scale;
                pos = g_debugCameraTransform.pos;
                rot = Transform_GetEulerAnglesDegrees(&g_debugCameraTransform);
            }
            else
            {
                rotation = ecT->t.rotation;
                inputRot = g_debugInput.degrees;
                scale = ecT->t.scale;
                pos = ecT->t.pos;
                rot = Transform_GetEulerAnglesDegrees(&ecT->t);
                if (isnan(rot.x))
                {
                    //App_ErrorStop();
                }
            }
                
                
                // mRot.z = atan2f(m[1], m[5]);
                // mRot.y = atan2f(m[8], m[10]);
                // mRot.x = -asinf(m[9]);
                
                //AngleVectors vectors = {};
                h.length = sprintf_s(gs->debugString, gs->debugStringCapacity,
"Game.DLL:\nTimeDelta: %3.7f\n\
Input Rot: %3.3f, %3.3f, %3.3f\n\
-- Debug Transform --\n\
pos: %3.3f, %3.3f, %3.3f\n\
euler: %3.3f, %3.3f, %3.3f\n\
scale: %3.3f, %3.3f, %3.3f\n\
Rotation:\n\
(X0) %3.3f, (Y0) %3.3f, (Z0) %3.3f\n\
(X1) %3.3f, (Y1) %3.3f, (Z1) %3.3f\n\
(X2) %3.3f, (Y2) %3.3f, (Z2) %3.3f\n\
",
                                     time->deltaTime,
                                     inputRot.x, inputRot.y, inputRot.z,
                                     pos.x, pos.y, pos.z,
                                     rot.x, rot.y, rot.z,
                                     //scale.x, scale.y, scale.z,
                                     scale.x, scale.y, scale.z,
                                     rotation.xAxisX, rotation.yAxisX, rotation.zAxisX,
                                     rotation.xAxisY, rotation.yAxisY, rotation.zAxisY,
                                     rotation.xAxisZ, rotation.yAxisZ, rotation.zAxisZ
                                     );
            //

        }
        break;

        case GAME_DEBUG_MODE_PHYSICS:
        {
            // char* str;
            // u32 numChars;
            Phys_GetDebugString(&h.chars, &h.length);
            
        } break;

        case GAME_DEBUG_MODE_ACTOR_INPUT:
        {
            h.length = Game_DebugWriteActiveActorInput(gs, gs->debugString, gs->debugStringCapacity);
        } break;

        case GAME_DEBUG_MODE_INPUT:
        {
            //
            i32 count = 2048;
            i32 written = 0;
            //char str[count];
            char* writePtr = gs->debugString;
            written += sprintf_s(writePtr, count, "--- RAW INPUT ---\n");
			writePtr += written;
            for (i32 i = 0; i < g_inputActions.count; ++i)
            {
                InputAction* action = &g_inputActions.actions[i];
                Assert(written < count);
                written += sprintf_s(writePtr, (count - written),
                    "%d: VAL %d CODE %d FRAME %d NAME: %s\n",
                    i,
                    action->value,
                    action->keyCode,
                    action->lastFrame,
					action->label
                );
				writePtr = gs->debugString + written;
            }
            h.length = written;
        } break;

        default:
        {
            h.length = 0;
            return h;
        }
        break;
    }
#endif
    return h;
}
