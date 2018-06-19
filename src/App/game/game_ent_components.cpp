#pragma once

#include "game.h"
#include "../common/com_module.h"

// void Game_UpdateAI(GameTime* time)
// {
//     for (i32 i = 0; i < g_num_brains; ++i)
//     {
//         EntComp_TestBrain* brain = &g_game_brains[i];
//         Ent* ent = Ent_GetEntityByIndex(brain->entId.index);
//         Assert(ent != NULL);
//         ent->transform.rot.y += 90 * time->deltaTime;
//     }
// }

Ent* Game_FindEntityByLabel(GameState* gs, char* queryLabel)
{
    for (u32 i = 0; i < gs->labelList.max; ++i)
    {
        EC_Label* entLabel = &gs->labelList.items[i];
        if (!COM_CompareStrings(entLabel->label, queryLabel))
        {
            return Ent_GetEntity(&gs->entList, &entLabel->entId);
        }
    }
    return NULL;
}

void SV_SpawnTestBullet(GameState* gs, f32 x, f32 y, f32 z, f32 pitchDegrees, f32 yawDegrees)
{
    Cmd_SpawnProjectile cmd = {};
    cmd.spawn.entityId = Ent_ReserveFreeEntity(&gs->entList);
    cmd.spawn.factoryType = 9999;   // Not used yet.
    cmd.spawn.pos.x = x;
    cmd.spawn.pos.y = y;
    cmd.spawn.pos.z = z;
    cmd.spawn.rot.y = yawDegrees;
    cmd.spawn.rot.x = pitchDegrees;
    cmd.speed = TEST_PROJECTILE_SPEED;
    printf("SV Creating prj command, entID: %d/%d\n",
        cmd.spawn.entityId.iteration,
        cmd.spawn.entityId.index
    );
    App_EnqueueCmd((u8*)&cmd, CMD_TYPE_SPAWN_PROJECTILE, sizeof(Cmd_SpawnProjectile));
#if 0
    //Ent* ent = Ent_GetFreeEntity(&gs->entList);
    EntId id = Ent_ReserveFreeEntity(&gs->entList);
    Ent* ent = Ent_GetAndAssign(&gs->entList, &id);
    
    //M4x4_SetToIdentity(ent->transform.matrix.cells);
    //ent->transform.matrix.wAxis = originT->matrix.wAxis;

    // Copy position and rotation
    ent->transform.pos.x = x;
    ent->transform.pos.y = y;
    ent->transform.pos.z = z;
    //Transform_SetScale(&ent->transform, 0.1f, 0.1f, 0.5f);
    Transform_SetScale(&ent->transform, 0.3f, 0.3f, 0.3f);
    Transform_RotateY(&ent->transform, yawDegrees * DEG2RAD);
    Transform_RotateX(&ent->transform, pitchDegrees * DEG2RAD);
    
    EC_Renderer* rend = EC_AddRenderer(gs, ent);
    //RendObj_SetAsMesh(&rend->rendObj, &g_meshSpike, 1, 1, 1, AppGetTextureIndexByName("BAL1A0.bmp"));
    RendObj_SetAsBillboard(&rend->rendObj, 1, 1, 1, AppGetTextureIndexByName("textures\\BAL1A0.bmp"));
    rend->rendObj.flags = 0 | RENDOBJ_FLAG_DEBUG;

    //Vec4 scale = M4x4_GetScale(ent->transform.matrix.cells);

    EC_Projectile* prj = EC_AddProjectile(gs, ent);
    prj->tock = 4.0f;
    prj->tick = prj->tock;
    
    prj->move.x = -ent->transform.rotation.zAxis.x * TEST_PROJECTILE_SPEED;
    prj->move.y = -ent->transform.rotation.zAxis.y * TEST_PROJECTILE_SPEED;
    prj->move.z = -ent->transform.rotation.zAxis.z * TEST_PROJECTILE_SPEED;
    // move projectile forward a little
    ent->transform.pos.x += -ent->transform.rotation.zAxis.x * 1;
    ent->transform.pos.y += -ent->transform.rotation.zAxis.y * 1;
    ent->transform.pos.z += -ent->transform.rotation.zAxis.z * 1;
    
    prj->tick = 1.0f;
    prj->tock = 1.0f;
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
    prj->tock = 4.0f;
    prj->tick = prj->tock;
    
    prj->move.x = -ent->transform.rotation.zAxis.x * TEST_PROJECTILE_SPEED;
    prj->move.y = -ent->transform.rotation.zAxis.y * TEST_PROJECTILE_SPEED;
    prj->move.z = -ent->transform.rotation.zAxis.z * TEST_PROJECTILE_SPEED;
    // move projectile forward a little
    ent->transform.pos.x += -ent->transform.rotation.zAxis.x * 1;
    ent->transform.pos.y += -ent->transform.rotation.zAxis.y * 1;
    ent->transform.pos.z += -ent->transform.rotation.zAxis.z * 1;
    
    prj->tick = 1.0f;
    prj->tock = 1.0f;
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
    Vec3 move = col->velocity;

    Vec3 input = {};
    Vec3 moveForce = {};

    u8 applyingInput = 0;
    if (motor->input.buttons & ACTOR_INPUT_MOVE_FORWARD)
    {
        applyingInput = 1;
        input.z -= 1;
    }
    if (motor->input.buttons & ACTOR_INPUT_MOVE_BACKWARD)
    {
        applyingInput = 1;
        input.z += 1;
    }
    if (motor->input.buttons & ACTOR_INPUT_MOVE_LEFT)
    {
        applyingInput = 1;
        input.x -= 1;
    }
    if (motor->input.buttons & ACTOR_INPUT_MOVE_RIGHT)
    {
        applyingInput = 1;
        input.x += 1;
    }

    Vec3_Normalise(&input);

    if (motor->input.buttons & ACTOR_INPUT_MOVE_UP && col->isGrounded)
    {
        move.y = 10;// * deltaTime;
        //printf("Apply up force: %.2f\n", move.y);
    }
    
	f32 radiansForward = motor->input.degrees.y * DEG2RAD;
	f32 radiansLeft = (motor->input.degrees.y + 90) * DEG2RAD;

    
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
    if (motor->tick <= 0)
    {
        if (motor->input.buttons & ACTOR_INPUT_ATTACK)
        {
            motor->tick = 0.1f;
            Ent* ent = Ent_GetEntityById(&gs->entList, &motor->entId);
            //Transform* t = &g_worldScene.cameraTransform;
            Transform* t = &ent->transform;

            SV_SpawnTestBullet(
                gs,
                t->pos.x,
                t->pos.y,
                t->pos.z,
                motor->input.degrees.x,
                motor->input.degrees.y
            );
            f32 spread = 2;
            f32 pitchOffset;
            f32 yawOffset;
            pitchOffset = COM_Randf32() * (spread - -spread) + -spread;
            yawOffset = COM_Randf32() * (spread - -spread) + -spread;
            SV_SpawnTestBullet(
                gs,
                t->pos.x,
                t->pos.y,
                t->pos.z,
                motor->input.degrees.x + pitchOffset,
                motor->input.degrees.y + yawOffset
            );
 
            pitchOffset = COM_Randf32() * (spread - -spread) + -spread;
            yawOffset = COM_Randf32() * (spread - -spread) + -spread;
            SV_SpawnTestBullet(
                gs,
                t->pos.x,
                t->pos.y,
                t->pos.z,
                motor->input.degrees.x + pitchOffset,
                motor->input.degrees.y + yawOffset
            );
            //printf("GAME Spawn bullet pitch %.1f, yaw %.1f\n", motor->input.degrees.x, motor->input.degrees.y);
        }
    }
    else
    {
        motor->tick -= deltaTime;
    }
}

// returns num chars written
i32 Game_DebugWriteActiveActorInput(GameState* gs, char* buf, i32 maxChars)
{
    i32 written = 0;
    for (u32 i = 0; i < gs->actorMotorList.max; ++i)
    {
        EC_ActorMotor* motor = &gs->actorMotorList.items[i];
        if (motor->inUse == 0) { continue; }

        written += sprintf_s(
            buf,
            maxChars,
"Ent %d/%d. L: %.1f, %.1f, %.1f Mov F/B/L/R/U/D: %d/%d/%d/%d/%d/%d\n\
ATK: %d TICK: %.2f SPEED: %.2f\n",
            motor->entId.iteration,
            motor->entId.index,
            motor->input.degrees.x,
            motor->input.degrees.y,
            motor->input.degrees.z,
            (motor->input.buttons & ACTOR_INPUT_MOVE_FORWARD),
            (motor->input.buttons & ACTOR_INPUT_MOVE_BACKWARD),
            (motor->input.buttons & ACTOR_INPUT_MOVE_LEFT),
            (motor->input.buttons & ACTOR_INPUT_MOVE_RIGHT),
            (motor->input.buttons & ACTOR_INPUT_MOVE_UP),
            (motor->input.buttons & ACTOR_INPUT_MOVE_DOWN),
            (motor->input.buttons & ACTOR_INPUT_ATTACK),
            motor->tick,
            motor->debugCurrentSpeed
        );

        //EC_Collider* col = EC_FindCollider(&motor->entId, gs);
        //Assert(col != NULL);
        //Ent* ent = Ent_GetEntityById(&gs->entList, &col->entId);
        //Assert(ent != NULL);
        

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
        if (motor->inUse == 0) { continue; }
        EC_Collider* col = EC_FindCollider(gs, &motor->entId);
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

///////////////////////////////////////////////////////////////////
// Projectiles
///////////////////////////////////////////////////////////////////
void Game_UpdateProjectiles(GameState* gs, GameTime* time)
{
    for (u32 i = 0; i < gs->projectileList.max; ++i)
    {
        EC_Projectile* prj = &gs->projectileList.items[i];
        if (prj->inUse == 0) { continue; }

        Ent* e = Ent_GetEntityByIndex(&gs->entList, prj->entId.index);

        if (prj->tick <= 0.0f)
        {
            // Delete
            Ent_MarkForFree(e);
            Cmd_RemoveEntity cmd = {};
            cmd.entId = e->entId;
            App_EnqueueCmd((u8*)&cmd, CMD_TYPE_REMOVE_ENT, sizeof(Cmd_RemoveEntity));
            //Ent_Free(gs, e);
        }
        else
        {
            prj->tick -= time->deltaTime;
        }

        Transform* t = &e->transform;

        Vec3 newPos = { prj->move.x, prj->move.y, prj->move.z};

        Phys_RayTest(t->pos.x, t->pos.y, t->pos.z, newPos.x, newPos.y, newPos.z);

        t->pos.x += prj->move.x * time->deltaTime;
        t->pos.y += prj->move.y * time->deltaTime;
        t->pos.z += prj->move.z * time->deltaTime;
    }
}

///////////////////////////////////////////////////////////////////
// Colliders
///////////////////////////////////////////////////////////////////

// Handled in physics engine now
void Game_UpdateColliders(GameState* gs, GameTime* time)
{
    #if 0
    u32 currentFrame = time->frameNumber;
    for (u32 i = 0; i < gs->colliderList.max; ++i)
    {
        EC_Collider* a = &gs->colliderList.items[i];
        if (a->inUse == 0) { continue; }
        Ent* entA = Ent_GetEntityByIndex(&gs->entList, a->entId.index);
        Transform* transA = &entA->transform;
        // Move collider
        // transA->pos.x += a->velocity.x * time->deltaTime;
        // transA->pos.y += a->velocity.y * time->deltaTime;
        // transA->pos.z += a->velocity.z * time->deltaTime;
        
        // for (u32 j = 0; j < gs->colliderList.max; ++j)
        // {
        //     if (i == j) { continue; }
        //     EC_Collider* b = &gs->colliderList.items[j];
        //     if (b->inUse == 0) { continue; }
        //     Ent* entB = Ent_GetEntityByIndex(&gs->entList, b->entId.index);
        //     Transform* transB = &entB->transform;

        //     if (AABBVsAABB(
        //         transA->pos.x,
        //         transA->pos.y,
        //         transA->pos.z,
        //         transB->pos.x,
        //         transB->pos.y,
        //         transB->pos.z,
        //         a->size.x / 2, a->size.y / 2, a->size.z / 2,
        //         b->size.x / 2, b->size.y / 2, b->size.z / 2

        //     ))
        //     {
        //         a->lastFrameOverlapping = currentFrame;
        //         b->lastFrameOverlapping = currentFrame;
        //     }
        // }
    }
    #endif
}

void Game_DrawColliderAABBs(GameState* gs, GameTime* time, RenderScene* scene)
{
    for (u32 i = 0; i < gs->colliderList.max; ++i)
    {
        EC_Collider* collider = &gs->colliderList.items[i];
        if (collider->inUse == 1)
        {
            Ent* ent = Ent_GetEntityByIndex(&gs->entList, collider->entId.index);
            RenderListItem* item = &scene->sceneItems[scene->numObjects];
            scene->numObjects++;
            item->transform = ent->transform;
            Transform_SetScale(&item->transform, 1, 1, 1);
            //if (collider->lastFrameOverlapping == time->frameNumber)
            if (collider->isGrounded)
            {
                RendObj_SetAsAABB(&item->obj, collider->size.x, collider->size.y, collider->size.z, 1, 0, 0);
            }
            else
            {
                RendObj_SetAsAABB(&item->obj, collider->size.x, collider->size.y, collider->size.z, 0, 1, 0);
            }
            
        }
    }
}

void Game_UpdateUI(GameState* gs, GameTime* time)
{
    #if 0
    f32 speed = 8.0f;
    Ent* ent = &gs->entList.items[0];
    if (input->yawLeft)
    {
        ent->transform.pos.x -= speed * time->deltaTime;
    }
    if (input->yawRight)
    {
        ent->transform.pos.x += speed * time->deltaTime;
    }

    if (input->pitchUp)
    {
        ent->transform.pos.y += speed * time->deltaTime;
    }
    if (input->pitchDown)
    {
        ent->transform.pos.y -= speed * time->deltaTime;
    }

    if (input->rollLeft)
    {
        ent->transform.pos.z -= speed * time->deltaTime;
    }
    if (input->rollRight)
    {
        ent->transform.pos.z += speed * time->deltaTime;
    }
    #endif
}
