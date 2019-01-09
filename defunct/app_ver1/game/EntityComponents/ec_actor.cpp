#pragma once

#include "game.h"

f32 friction = 7;
f32 accelerate = 100;

internal Vec3 Game_CalcHorizontalMoveA(Vec3 *input, f32 yawDegrees)
{
    Vec3 moveForce = {};
    // Not doing this REDUCES movement speed when holding forward + sideways
    Vec3_Normalise(input);

    f32 radiansForward = yawDegrees * DEG2RAD;
    f32 radiansLeft = (yawDegrees + 90) * DEG2RAD;

    Vec4 left = {};
    Vec4 up = {};
    Vec4 forward = {};
    // Calculate forward and left vectors, multiple by input -1, 0 or +1
    // to cancel out or flip direction if necessary

    forward.x = sinf(radiansForward);
    forward.y = 0;
    forward.z = cosf(radiansForward);

    left.x = sinf(radiansLeft);
    left.y = 0;
    left.z = cosf(radiansLeft);

    moveForce.x = (forward.x * input->z) + (left.x * input->x);
    moveForce.z = (forward.z * input->z) + (left.z * input->x);
    return moveForce;
}
// Quake 1 style movement.
Vec3 MoveGround(Vec3* accelDir,
    Vec3* prevVelocity,
    u8 onGround,
    f32 acceleration,
    f32 maxVelocity,
    f32 deltaTime)
{
    f32 speed = Vec3_Magnitude(prevVelocity);
    // Apply friction
    // TODO: Forcing onGround to true for now as bunny hop acceleration requires tweaking
    //onGround = 1;
    
    if (onGround && speed != 0) // avoid divide by zero ninny
    {
        float drop = speed * friction * deltaTime;
        f32 frictionScalar = ZMAX(speed - drop, 0) / speed;
        // scale velocity based on friction
        prevVelocity->x *= frictionScalar;
        prevVelocity->z *= frictionScalar;
    }
    else
    {
        acceleration *= 0.2f;
    }
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

// TODO: Doom style "incorrect" movement (ie strafe-running)
#if 0
Vec3 MoveGround(
    Vec3 *accelDir,
    Vec3 *prevVelocity,
    u8 onGround,
    f32 acceleration,
    f32 maxVelocity,
    f32 deltaTime)
{
    Vec3 result = {};
    //f32 currentVelocity = Vec3_Magnitudef(prevVelocity->x, 0, prevVelocity->z);
    //f32 current2FullRatio = 1 - (currentVelocity * maxVelocity);

    result.x = accelDir->x * maxVelocity;
    result.z = accelDir->z * maxVelocity;
    return result;
}
#endif

void ApplyActorMotorInput(
    GameScene *gs,
    EC_ActorMotor *motor,
    EC_Collider *col,
    f32 deltaTime)
{
    Vec3 move = col->state.velocity;

    Vec3 input = {};
    Vec3 moveForce = {};
    Vec3 degrees = motor->state.input.degrees;

    u32 buttons = motor->state.input.buttons;

    u8 applyingInput = 0;
    if (buttons & ACTOR_INPUT_MOVE_FORWARD)
    {
        applyingInput = 1;
        input.z -= 1;
    }
    if (buttons & ACTOR_INPUT_MOVE_BACKWARD)
    {
        applyingInput = 1;
        input.z += 1;
    }
    if (buttons & ACTOR_INPUT_MOVE_LEFT)
    {
        applyingInput = 1;
        input.x -= 1;
    }
    if (buttons & ACTOR_INPUT_MOVE_RIGHT)
    {
        applyingInput = 1;
        input.x += 1;
    }

    moveForce = Game_CalcHorizontalMoveA(&input, degrees.y);
    Vec3 moveResult = MoveGround(
    &moveForce,
    &move,
    col->isGrounded,
    motor->state.runAcceleration,
    motor->state.runSpeed,
    deltaTime);

    // jumping. weeeeeeeee
    if (col->isGrounded)
    {
        motor->state.flags &= ~EC_ACTOR_MOTOR_FLAG_MOVE_SPECIAL_LOCKED;

        if (buttons & ACTOR_INPUT_MOVE_UP)
        {
            move.y = 7.5f;
            //printf("Apply up force: %.2f\n", move.y);
        }
    }
    

    move.x = moveResult.x;
    move.z = moveResult.z;
    motor->debugCurrentSpeed = Vec3_Magnitude(&move);

    if (
        (buttons & ACTOR_INPUT_MOVE_SPECIAL1)
        && !col->isGrounded
        && (motor->state.flags & EC_ACTOR_MOTOR_FLAG_MOVE_SPECIAL_LOCKED) == 0
        )
    {
        motor->state.flags |= EC_ACTOR_MOTOR_FLAG_MOVE_SPECIAL_LOCKED;
        Vec3 forward = Vec3_ForwardFromAngles(degrees.y, degrees.x, 16);
        //move.x += forward.x;
        //move.y += forward.y;
        //move.z += forward.z;

        move.x = forward.x;
        move.y = forward.y;
        move.z = forward.z;
    }

    PhysCmd_ChangeVelocity(col->shapeId, move.x, move.y, move.z);

    Ticker *ticker = &motor->state.ticker;
    // Attack
    if (ticker->tick <= 0)
    {
        if (motor->state.input.buttons & ACTOR_INPUT_ATTACK)
        {
            ticker->tickMax = motor->state.attack1Reload;
            ticker->tick = ticker->tickMax;
            motor->state.animStyle = motor->state.attack1AnimStyle;
            //Ent* ent = Ent_GetEntityById(&gs->entList, &motor->header.entId);
            EC_Transform *ecTrans = EC_FindTransform(gs, &motor->header.entId);
            //Transform* t = &g_worldScene.cameraTransform;
            //Transform* t = &ent->transform;
            Transform *t = &ecTrans->t;
            Ent *self = Ent_GetEntityById(&gs->entList, &motor->header.entId);

            AttackInfo info = {};
            info.type = motor->state.attack1Type;
            info.team = self->team;
            info.origin = t->pos;
            info.source = motor->header.entId;
            info.yawDegrees = motor->state.input.degrees.y;
            info.pitchDegrees = motor->state.input.degrees.x;
            SV_FireAttack(gs, &info);
        }
        if (motor->state.input.buttons & ACTOR_INPUT_ATTACK2)
        {
            ticker->tickMax = motor->state.attack2Reload;
            ticker->tick = ticker->tickMax;
            motor->state.animStyle = motor->state.attack2AnimStyle;
            //Ent* ent = Ent_GetEntityById(&gs->entList, &motor->header.entId);
            EC_Transform *ecTrans = EC_FindTransform(gs, &motor->header.entId);
            //Transform* t = &g_worldScene.cameraTransform;
            //Transform* t = &ent->transform;
            Transform *t = &ecTrans->t;
            Ent *self = Ent_GetEntityById(&gs->entList, &motor->header.entId);

            AttackInfo info = {};
            info.type = motor->state.attack2Type;
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
i32 Game_DebugWriteActiveActorInput(GameScene *gs, char *buf, i32 maxChars)
{
    i32 wroteSomething = 0;
    i32 written = 0;
    char *ptrWrite = buf;
    for (u32 i = 0; i < gs->actorMotorList.max; ++i)
    {
        EC_ActorMotor *motor = &gs->actorMotorList.items[i];
        if (motor->header.inUse == 0)
        {
            continue;
        }
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
            motor->debugCurrentSpeed);
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
void Game_UpdateActorMotors(GameScene* scene, GameTime *time)
{
    for (u32 i = 0; i < scene->actorMotorList.max; ++i)
    {
        EC_ActorMotor *motor = &scene->actorMotorList.items[i];
        if (motor->header.inUse == 0)
        {
            continue;
        }
        EC_Collider *col = EC_FindCollider(scene, &motor->header.entId);
        Assert(col != NULL);
        ApplyActorMotorInput(scene, motor, col, time->deltaTime);
    }
}
