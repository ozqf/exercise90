#pragma once

#include "game.h"
#include "../common/com_module.h"

void SV_SpawnTestBullet(GameState* gs, EntId source, f32 x, f32 y, f32 z, f32 pitchDegrees, f32 yawDegrees)
{
    EntitySpawnOptions options = {};
    options.pos.x = x;
    options.pos.y = y;
    options.pos.z = z;
    options.source = source;
    
    options.vel = Vec3_ForwardFromAngles(yawDegrees, pitchDegrees, 100);
    
    Game_WriteSpawnCmd(gs, ENTITY_TYPE_PROJECTILE, &options);

    #if 0
    Cmd_EntityState cmd = {};
    cmd.entityId = Ent_ReserveFreeEntity(&gs->entList);
    cmd.factoryType = ENTITY_TYPE_PROJECTILE;
    cmd.pos.x = x;
    cmd.pos.y = y;
    cmd.pos.z = z;
    cmd.rot.y = yawDegrees;
    cmd.rot.x = pitchDegrees;
    cmd.moveSpeed = TEST_PROJECTILE_SPEED;

    cmd.vel = Vec3_ForwardFromAngles(yawDegrees, pitchDegrees, cmd.moveSpeed);

    cmd.ticker.tickMax = 1.0f;
    cmd.ticker.tick = 0.5f;

    if (g_verbose)
    {
        printf("SV Creating prj command, entID: %d/%d\n",
            cmd.entityId.iteration,
            cmd.entityId.index
        );
    }
    APP_WRITE_CMD(0, CMD_TYPE_ENTITY_STATE, 0, cmd);
    //App_WriteGameCmd((u8*)&cmd, CMD_TYPE_ENTITY_STATE, sizeof(Cmd_EntityState));
    #endif
}

void Game_SpawnTestBulletOld(GameState* gs, Transform* originT)
{
    //Ent* ent = Ent_GetFreeEntity(&gs->entList);
    EntId id = Ent_ReserveFreeEntity(&gs->entList);
    Ent* ent = Ent_GetAndAssign(&gs->entList, &id);
    
    //M4x4_SetToIdentity(ent->transform.matrix.cells);
    //ent->transform.matrix.wAxis = originT->matrix.wAxis;

    // Copy position and rotation
    ent->transform = *originT;
    //Transform_SetScale(&ent->transform, 0.1f, 0.1f, 0.5f);
    Transform_SetScale(&ent->transform, 0.3f, 0.3f, 0.3f);
    
    EC_Renderer* rend = EC_AddRenderer(gs, ent);
    //RendObj_SetAsMesh(&rend->rendObj, &g_meshSpike, 1, 1, 1, AppGetTextureIndexByName("BAL1A0.bmp"));
    RendObj_SetAsBillboard(&rend->rendObj, 1, 1, 1, AppGetTextureIndexByName("BAL1A0.bmp"));
    rend->rendObj.flags = 0 | RENDOBJ_FLAG_DEBUG;

    //Vec4 scale = M4x4_GetScale(ent->transform.matrix.cells);

    EC_Projectile* prj = EC_AddProjectile(gs, ent);
    prj->state.ticker.tickMax = 1.0f;
    prj->state.ticker.tick = prj->state.ticker.tickMax;
    
    prj->state.move.x = -ent->transform.rotation.zAxis.x * TEST_PROJECTILE_SPEED;
    prj->state.move.y = -ent->transform.rotation.zAxis.y * TEST_PROJECTILE_SPEED;
    prj->state.move.z = -ent->transform.rotation.zAxis.z * TEST_PROJECTILE_SPEED;
    // move projectile forward a little
    ent->transform.pos.x += -ent->transform.rotation.zAxis.x * 1;
    ent->transform.pos.y += -ent->transform.rotation.zAxis.y * 1;
    ent->transform.pos.z += -ent->transform.rotation.zAxis.z * 1;
}

// public float friction = 8f;
// public float air_accelerate = 100f;
// public float ground_accelerate = 50f;
// public float max_velocity_ground = 8f;
// public float max_velocity_air = 2f;

f32 friction = 7;
f32 accelerate = 100;

Vec3 MoveGround(Vec3* accelDir, Vec3* prevVelocity, u8 onGround, f32 acceleration, f32 maxVelocity, f32  deltaTime)
{
    f32 speed = Vec3_Magnitude(prevVelocity);
    // Apply friction
    // TODO: Forcing onGround to true for now as bunny hop acceleration requires tweaking
    onGround = 1;
#if 1
    if (onGround && speed != 0) // avoid divide by zero ninny
    {
        float drop = speed * friction * deltaTime;
        f32 frictionScalar = ZMAX(speed - drop, 0) / speed;
        // scale velocity based on friction
        prevVelocity->x *= frictionScalar;
        prevVelocity->z *= frictionScalar;
    }
#endif


    // Accelerate
    float projectionVelocity = Vec3_DotProduct(prevVelocity, accelDir);
    float accelVel = acceleration * deltaTime;

    // If necessary truncate the velocity so the vector projection does not exceed maxVelocity
    if (projectionVelocity + accelVel > maxVelocity)
    {
        accelVel = maxVelocity - projectionVelocity;
    }

    // Avoid actively reducing speed
    if (accelVel < 0)
    {
        accelVel = 0;
    }

    // printf("Speed %.1f, AccelDir: %.1f, %.1f, prevVel: %.1f, %.1f, %.1f\n",
    //     speed,
    //     accelDir->x,
    //     accelDir->z,
    //     prevVelocity->x,
    //     prevVelocity->y,
    //     prevVelocity->z
    // );

    // printf("ProjVel: %.1f, AccelVel: %.1f, Acceleration: %.1f, max: %.1f\n",
    //     projectionVelocity,
    //     accelVel,
    //     acceleration,
    //     maxVelocity
    // );


    Vec3 result = {};
    result.x = prevVelocity->x + accelDir->x * accelVel;
    //result.y = prevVelocity->y + accelDir->y * accelVel;
    result.z = prevVelocity->z + accelDir->z * accelVel;
    //printf(" Result: %.1f, %.1f, %.1f\n", result.x, result.y, result.z);
    return result;
}

inline void ApplyActorMotorInput(GameState* gs, EC_ActorMotor* motor, EC_Collider* col, f32 deltaTime)
{
    Vec3 move = col->state.velocity;

    Vec3 input = {};
    Vec3 moveForce = {};

    u8 applyingInput = 0;
    if (motor->state.input.buttons & ACTOR_INPUT_MOVE_FORWARD)
    {
        applyingInput = 1;
        input.z -= 1;
    }
    if (motor->state.input.buttons & ACTOR_INPUT_MOVE_BACKWARD)
    {
        applyingInput = 1;
        input.z += 1;
    }
    if (motor->state.input.buttons & ACTOR_INPUT_MOVE_LEFT)
    {
        applyingInput = 1;
        input.x -= 1;
    }
    if (motor->state.input.buttons & ACTOR_INPUT_MOVE_RIGHT)
    {
        applyingInput = 1;
        input.x += 1;
    }

    Vec3_Normalise(&input);

    if (motor->state.input.buttons & ACTOR_INPUT_MOVE_UP && col->isGrounded)
    {
        move.y = 10;// * deltaTime;
        //printf("Apply up force: %.2f\n", move.y);
    }
    
	f32 radiansForward = motor->state.input.degrees.y * DEG2RAD;
	f32 radiansLeft = (motor->state.input.degrees.y + 90) * DEG2RAD;

    
	Vec4 left = {};
	Vec4 up = {};
	Vec4 forward = {};
    #if 1
    forward.x = sinf(radiansForward);
	forward.y = 0;
	forward.z = cosf(radiansForward);

	left.x = sinf(radiansLeft);
	left.y = 0;
	left.z = cosf(radiansLeft);

    moveForce.x = (forward.x * input.z) + (left.x * input.x);
    moveForce.z = (forward.z * input.z) + (left.z * input.x);

    Vec3 moveResult = MoveGround(&moveForce, &move, col->isGrounded, 100, 12, deltaTime);
    move.x = moveResult.x;
    move.z = moveResult.z;
    motor->debugCurrentSpeed = Vec3_Magnitude(&move);

    //move = moveForce;
    #endif

    Phys_ChangeVelocity(col->shapeId, move.x, move.y, move.z);

    // Attack
    if (motor->state.ticker.tick <= 0)
    {
        if (motor->state.input.buttons & ACTOR_INPUT_ATTACK)
        {
            motor->state.ticker.tick = 0.1f;
            Ent* ent = Ent_GetEntityById(&gs->entList, &motor->header.entId);
            //Transform* t = &g_worldScene.cameraTransform;
            Transform* t = &ent->transform;

            SV_SpawnTestBullet(
                gs,
                motor->header.entId,
                t->pos.x,
                t->pos.y,
                t->pos.z,
                motor->state.input.degrees.x,
                motor->state.input.degrees.y
            );
            #if 1
            f32 spread = 2;
            f32 pitchOffset;
            f32 yawOffset;
            pitchOffset = COM_Randf32() * (spread - -spread) + -spread;
            yawOffset = COM_Randf32() * (spread - -spread) + -spread;
            SV_SpawnTestBullet(
                gs,
                motor->header.entId,
                t->pos.x,
                t->pos.y,
                t->pos.z,
                motor->state.input.degrees.x + pitchOffset,
                motor->state.input.degrees.y + yawOffset
            );
 
            pitchOffset = COM_Randf32() * (spread - -spread) + -spread;
            yawOffset = COM_Randf32() * (spread - -spread) + -spread;
            SV_SpawnTestBullet(
                gs,
                motor->header.entId,
                t->pos.x,
                t->pos.y,
                t->pos.z,
                motor->state.input.degrees.x + pitchOffset,
                motor->state.input.degrees.y + yawOffset
            );
            #endif
            //printf("GAME Spawn bullet pitch %.1f, yaw %.1f\n", motor->input.degrees.x, motor->input.degrees.y);
        }
    }
    else
    {
        motor->state.ticker.tick -= deltaTime;
    }
}

// returns num chars written
i32 Game_DebugWriteActiveActorInput(GameState* gs, char* buf, i32 maxChars)
{
    i32 wroteSomething = 0;
    i32 written = 0;
    char* ptrWrite = buf;
    for (u32 i = 0; i < gs->actorMotorList.max; ++i)
    {
        EC_ActorMotor* motor = &gs->actorMotorList.items[i];
        if (motor->header.inUse == 0) { continue; }
        wroteSomething = 1;
        written += sprintf_s(
            ptrWrite,
            (maxChars - written),
"Ent %d/%d. L: %.1f, %.1f, %.1f Mov F/B/L/R/U/D: %d/%d/%d/%d/%d/%d\n\
ATK: %d TICK: %.2f SPEED: %.2f\n",
            motor->header.entId.iteration,
            motor->header.entId.index,
            motor->state.input.degrees.x,
            motor->state.input.degrees.y,
            motor->state.input.degrees.z,
            (motor->state.input.buttons & ACTOR_INPUT_MOVE_FORWARD),
            (motor->state.input.buttons & ACTOR_INPUT_MOVE_BACKWARD),
            (motor->state.input.buttons & ACTOR_INPUT_MOVE_LEFT),
            (motor->state.input.buttons & ACTOR_INPUT_MOVE_RIGHT),
            (motor->state.input.buttons & ACTOR_INPUT_MOVE_UP),
            (motor->state.input.buttons & ACTOR_INPUT_MOVE_DOWN),
            (motor->state.input.buttons & ACTOR_INPUT_ATTACK),
            motor->state.ticker.tick,
            motor->debugCurrentSpeed
        );
        ptrWrite = buf + written;
        //EC_Collider* col = EC_FindCollider(&motor->entId, gs);
        //Assert(col != NULL);
        //Ent* ent = Ent_GetEntityById(&gs->entList, &col->entId);
        //Assert(ent != NULL);
        

    }

    if (!wroteSomething)
    {
        sprintf_s(ptrWrite, maxChars, "No actor motor found\n");
    }

    return written;
}

///////////////////////////////////////////////////////////////////
// Player
///////////////////////////////////////////////////////////////////
void Game_UpdateActorMotors(GameState* gs, GameTime* time)
{
    for (u32 i = 0; i < gs->actorMotorList.max; ++i)
    {
        EC_ActorMotor* motor = &gs->actorMotorList.items[i];
        if (motor->header.inUse == 0) { continue; }
        EC_Collider* col = EC_FindCollider(gs, &motor->header.entId);
        Assert(col != NULL);
        //Ent* ent = Ent_GetEntityById(&gs->entList, &col->entId);
        //Assert(ent != NULL);
        ApplyActorMotorInput(gs, motor, col, time->deltaTime);

    }
    #if 0
    static float fireTick = 0;
    Ent* ent = Ent_GetEntityByIndex(&gs->entList, gs->playerEntityIndex);

    if (fireTick <= 0)
    {
        if (Input_GetActionValue(actions, "Attack 1"))
        {
            fireTick = 0.1f;
            Transform* t = &g_worldScene.cameraTransform;
            
            Game_SpawnTestBullet(gs, t);
        }
    }
    else
    {
        fireTick -= time->deltaTime;
    }
    #endif
}


