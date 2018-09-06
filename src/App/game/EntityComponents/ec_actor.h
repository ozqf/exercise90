#pragma once

#include "game.h"
#include "../common/com_module.h"

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

    Vec3 result = {};
    result.x = prevVelocity->x + accelDir->x * accelVel;
    //result.y = prevVelocity->y + accelDir->y * accelVel;
    result.z = prevVelocity->z + accelDir->z * accelVel;
    //printf(" Result: %.1f, %.1f, %.1f\n", result.x, result.y, result.z);
    return result;
}

inline void ApplyActorMotorInput(
    GameState* gs,
    EC_ActorMotor* motor,
    EC_Collider* col,
    f32 deltaTime
    )
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
        move.y = 7.5f;// * deltaTime;
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

    Vec3 moveResult = MoveGround(&moveForce, &move, col->isGrounded, motor->state.runAcceleration, motor->state.runSpeed, deltaTime);
    move.x = moveResult.x;
    move.z = moveResult.z;
    motor->debugCurrentSpeed = Vec3_Magnitude(&move);

    //move = moveForce;
    #endif

    PhysCmd_ChangeVelocity(col->shapeId, move.x, move.y, move.z);
    Ticker* ticker = &motor->state.ticker;
    // Attack
    if (ticker->tick <= 0)
    {
        if (motor->state.input.buttons & ACTOR_INPUT_ATTACK)
        {
            ticker->tick = ticker->tickMax;
            //Ent* ent = Ent_GetEntityById(&gs->entList, &motor->header.entId);
            EC_Transform* ecTrans = EC_FindTransform(gs, &motor->header.entId);
            //Transform* t = &g_worldScene.cameraTransform;
            //Transform* t = &ent->transform;
            Transform* t = &ecTrans->t;
            Ent* self = Ent_GetEntityById(&gs->entList, &motor->header.entId);
            
            AttackInfo info = {};
            info.type = motor->state.attackType;
            info.team = self->team;
            info.origin = t->pos;
            info.source = motor->header.entId;
            info.yawDegrees = motor->state.input.degrees.y;
            info.pitchDegrees = motor->state.input.degrees.x;
            SV_FireAttack(gs, &info);
        }
    }
    else
    {
        ticker->tick -= deltaTime;
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
        ApplyActorMotorInput(gs, motor, col, time->deltaTime);
    }
}
