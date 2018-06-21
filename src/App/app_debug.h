#pragma once

#include "app_module.cpp"

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

        case GAME_DEBUG_MODE_TRANSFORM:
        {
            Ent *ent = Ent_GetEntityByTag(&gs->entList, 10);
            M3x3 rotation = {};
            Vec3 inputRot = {};
            Vec3 pos = {};
            Vec3 rot = {};
            Vec3 scale = {};
            if (ent == NULL)
            {
                rotation = gs->cameraTransform.rotation;
                inputRot = g_debugInput.degrees;
                scale = gs->cameraTransform.scale;
                pos = gs->cameraTransform.pos;
                rot = Transform_GetEulerAnglesDegrees(&gs->cameraTransform);
            }
            else
            {
                rotation = ent->transform.rotation;
                inputRot = g_debugInput.degrees;
                scale = ent->transform.scale;
                pos = ent->transform.pos;
                rot = Transform_GetEulerAnglesDegrees(&ent->transform);
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
