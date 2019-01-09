#pragma once

#include "game.h"

#define AI_STATE_NULL 0
#define AI_STATE_TRACKING 1
#define AI_STATE_STUNNED 2
#define AI_STATE_BEGIN_ATTACK 3
#define AI_STATE_ATTACKING 4
#define AI_STATE_FINISH_ATTACK 5
#define AI_STATE_WANDER 6
#define AI_STATE_REV_UP_CHARGE 7
#define AI_STATE_CHARGING 8

struct AITargetInfo
{
    Vec3 selfPos;
    Vec3 targetPos;
    f32 dx, dy, dz;
    f32 pitchRadians, yawRadians;
    f32 flatMagnitude;
};

u8 AI_ValidateTargetById(GameScene* scene, EntId* id, i32 selfTeam)
{
    Ent* ent = Ent_GetEntityById(scene, id);
    if (!ent) { return 0; }
    if (ent->inUse != ENTITY_STATUS_IN_USE) { return 0; }
    if ((ent->componentBits & EC_BIT1_TRANSFORM) == 0) { return 0; }
    if (!Game_AttackIsValid(selfTeam, ent->team)) { return 0; }
    return 1;
}

u8 AI_AcquireAndValidateTarget(ClientList* clients, GameScene *scene, i32 selfTeam, Vec3 selfPos, EntId* id)
{
    Ent* ent = Ent_GetEntityById(scene, id);
    if (!ent)
    {
        id->value = 0;
        *id = AI_FindNearestPlayer(clients, scene, { selfPos.x, selfPos.y, selfPos.z });
    }
    if (id->value == 0) { return 0; }
    //  printf("AI Acquired %d/%d\n", id->iteration, id->index);
    ent = Ent_GetEntityById(scene, id);
	return AI_ValidateTargetById(scene, id, selfTeam);
}

void AI_BuildThinkInfo(GameScene* gs, EC_AIController* ai, AITargetInfo* info)
{
    EC_Transform* selfTrans = EC_FindTransform(gs, &ai->header.entId);
    EC_Transform* targetTrans = EC_FindTransform(gs, &ai->state.target);
    Assert(selfTrans);
    Assert(targetTrans);

    info->selfPos = selfTrans->t.pos;
    info->targetPos = targetTrans->t.pos;

    ////////////////////////////////////
    // flat plane angle
    info->dx = info->selfPos.x - info->targetPos.x;
    info->dy = info->selfPos.y - info->targetPos.y;
    info->dz = info->selfPos.z - info->targetPos.z;
    info->flatMagnitude = Vec3_Magnitudef(info->dx, 0, info->dz);
    info->yawRadians = atan2f(info->dx, info->dz);            
    ////////////////////////////////////
    // Pitch, look up/down angle
    info->pitchRadians = atan2f(info->dy, info->flatMagnitude);
}

void AI_ClearInput(GameScene* gs, EC_AIController* ai)
{
    EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
    motor->state.input.buttons = 0;
}

void AI_Reset(GameScene* gs, EC_AIController* ai)
{
    AI_ClearInput(gs, ai);
    ai->state.state = AI_STATE_NULL;
}

void AI_Stun(GameScene* gs, EC_AIController* ai, GameTime* time)
{
    AI_ClearInput(gs, ai);
    ai->state.state = AI_STATE_STUNNED;

}

void AI_ApplyLookAngles(EC_ActorMotor* m, f32 yawRadians, f32 pitchRadians)
{
    m->state.input.degrees.y = yawRadians * RAD2DEG;
    m->state.input.degrees.x = -(pitchRadians * RAD2DEG);
}

void AI_ReduceNextThink(EC_AIController* ai, GameTime* time, f32 newNextThinkTime)
{
    f32 diff = ai->state.nextThink - time->sessionEllapsed;
    if (diff < newNextThinkTime) { return; }
    ai->state.nextThink = time->sessionEllapsed + newNextThinkTime;
}

///////////////////////////////////////////////////////////////////////
// THINK - not always run
///////////////////////////////////////////////////////////////////////
i32 AI_Think(ClientList* clients, GameScene* gs, Ent* self, EC_AIController* ai, GameTime* time)
{
    //printf("AI Think %d\n", ai->state.state);
    switch (ai->state.state)
    {
        case AI_STATE_NULL:
        {
            if (AI_AcquireAndValidateTarget(clients, gs, self->team, { 0, 0, 0 }, &ai->state.target))
            {
                ai->state.state = AI_STATE_TRACKING;
                //printf("Acquired target. State: %d\n", ai->state.state);
            }
            else if (ai->state.target.value != 0)
            {
                ai->state.target.value = 0;
                AI_Reset(gs, ai);
            }
        } break;

        case AI_STATE_WANDER:
        {
            AI_ClearInput(gs, ai);
            ai->state.state = AI_STATE_TRACKING;
            // reset think time so think is performed immediately on next frame
            ai->state.nextThink = 0;
            return 0;
        } break;

        case AI_STATE_TRACKING:
        {
			AITargetInfo info = {};
			if (!AI_AcquireAndValidateTarget(clients, gs, self->team, { 0, 0, 0 }, &ai->state.target))
			{
				AI_Reset(gs, ai);
				return 1;
			}

            EC_Collider* col = EC_FindCollider(gs, &EC_GET_ID(ai));
            if (col != NULL && col->isGrounded == 0) { return 1; }

            AI_BuildThinkInfo(gs, ai, &info);
            if (info.flatMagnitude < 100)
            {
				if (ai->state.type == 0)
				{
					ai->state.state = AI_STATE_ATTACKING;
				}
				else
				{
					ai->state.state = AI_STATE_REV_UP_CHARGE;
				}
                ai->state.nextThink = time->sessionEllapsed + ai->state.atkSettings.startUpDelay;

                EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
                motor->state.input.buttons = 0;
                AI_ApplyLookAngles(motor, info.yawRadians, info.pitchRadians);
            }
        } break;

        case AI_STATE_REV_UP_CHARGE:
        {
            #if 1
            ai->state.state = AI_STATE_CHARGING;
            ai->state.nextThink = time->sessionEllapsed + ai->state.atkSettings.recoverDelay;
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            motor->state.runSpeed = 35;
            motor->state.runAcceleration = 200;
            motor->state.input.buttons |= ACTOR_INPUT_MOVE_FORWARD;
            #endif
            #if 0
            // debug charge behaviour
            ai->state.state = AI_STATE_CHARGING;
            ai->state.nextThink = time->sessionEllapsed + 9999999.0f;
            #endif
        } break;

        case AI_STATE_CHARGING:
        {
            ai->state.state = AI_STATE_TRACKING;
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            motor->state.runSpeed = 10;
            motor->state.runAcceleration = 100;
            ai->state.nextThink = time->sessionEllapsed + 1.0f;
        } break;
        
        // case AI_STATE_BEGIN_ATTACK:
        // {
        //     EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
        //     motor->state.input.buttons = 0;
        //     ai->state.state = AI_STATE_ATTACKING;
            
        // } break;
        
        case AI_STATE_ATTACKING:
        {
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            EC_Transform* selfTrans = EC_FindTransform(gs, &EC_GET_ID(ai));
            Assert(motor);
            Assert(selfTrans);

            if ((ai->state.atkSettings.flags & ATTACK_FLAG_NO_TARGET_TRACK) == 0)
            {
                i32 team = Ent_GetEntityById(gs, &ai->header.entId)->team;
                if (AI_ValidateTargetById(gs, &ai->state.target, team) == 0)
                {
                    // drop attack
                    AI_Reset(gs, ai);
                    break;
                }

                AITargetInfo info = {};
                AI_BuildThinkInfo(gs, ai, &info);
                AI_ApplyLookAngles(motor, info.yawRadians, info.pitchRadians);
            }

            AttackInfo atk = {};
            
            atk.type = ai->state.atkSettings.type;
            atk.origin = selfTrans->t.pos;
            atk.team = self->team;
            atk.source = EC_GET_ID(ai);
            atk.yawDegrees = motor->state.input.degrees.y;
            atk.pitchDegrees = motor->state.input.degrees.x;
            SV_FireAttack(gs, &atk);
            ai->state.attackCount++;
            if (ai->state.attackCount >= ai->state.atkSettings.repeatCount)
            {
                // finish
                ai->state.state = AI_STATE_TRACKING;
                ai->state.attackCount = 0;
                ai->state.nextThink = time->sessionEllapsed + ai->state.atkSettings.recoverDelay;
            }
            else
            {
                ai->state.nextThink = time->sessionEllapsed + ai->state.atkSettings.repeatDelay;
            }
        } break;
        
        case AI_STATE_STUNNED:
        {
            ai->state.state = AI_STATE_NULL;
        } break;
    }
    return 1;
}

///////////////////////////////////////////////////////////////////////
// TICK - Run every frame 
///////////////////////////////////////////////////////////////////////
void AI_Tick(ClientList* clients, GameScene* gs, Ent* self, EC_AIController* ai, GameTime* time)
{
    //printf("AI tock. State: %d\n", ai->state.state);
    if (ai->state.nextThink <= time->sessionEllapsed)
    {
        if (!AI_Think(clients, gs, self, ai, time))
        {
            return;
        }
    }
    #if 1
    switch (ai->state.state)
    {
        case AI_STATE_TRACKING:
        {
            if (!AI_AcquireAndValidateTarget(clients, gs, self->team, { 0, 0, 0 }, &ai->state.target))
            {
                AI_Reset(gs, ai);
                return;
            }
            EC_Collider* col = EC_FindCollider(gs, &EC_GET_ID(ai));
            if (col != NULL && col->isGrounded == 0) { return; }

            AITargetInfo info = {};
            AI_BuildThinkInfo(gs, ai, &info);
            ////////////////////////////////////
            // Apply inputs and angles to motor
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            u8 applyMove = (info.flatMagnitude > ai->state.minApproachDistance);
            
            if (applyMove)
            {
                motor->state.input.buttons |= ACTOR_INPUT_MOVE_FORWARD;
            }
            else
            {
                // disable bit
                motor->state.input.buttons &= ~ACTOR_INPUT_MOVE_FORWARD;
            }
            // crappy wander movement when close
            #if 0
            if (applyMove)
            {
                //AI_ClearInput(gs, ai);
                ai->state.state = AI_STATE_WANDER;
                motor->state.input.buttons |= ACTOR_INPUT_MOVE_FORWARD;
                info.yawRadians += (90 * DEG2RAD);
            }
            else
            {
                motor->state.input.buttons |= ACTOR_INPUT_MOVE_FORWARD;
            }
            #endif

            AI_ApplyLookAngles(motor, info.yawRadians, info.pitchRadians);
            
        } break;
		
		case AI_STATE_ATTACKING:
		{
			if ((ai->state.atkSettings.flags & ATTACK_FLAG_NO_TARGET_TRACK) == 0)
			{
				if (!AI_ValidateTargetById(gs, &ai->state.target, self->team))
				{
					AI_Reset(gs, ai);
					break;
				}
				EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
				Assert(motor);
	
				if ((ai->state.atkSettings.flags & ATTACK_FLAG_NO_TARGET_TRACK) == 0)
				{
					AITargetInfo info = {};
					AI_BuildThinkInfo(gs, ai, &info);
					AI_ApplyLookAngles(motor, info.yawRadians, info.pitchRadians);
				}
			}
		} break;
		
		case AI_STATE_REV_UP_CHARGE:
		{
			if (!AI_ValidateTargetById(gs, &ai->state.target, self->team))
			{
				//AI_Reset(gs, ai);
				break;
			}
			else
			{
				AITargetInfo info = {};
				AI_BuildThinkInfo(gs, ai, &info);
				
				EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
				motor->state.input.degrees.x += -(600 * time->deltaTime);
				motor->state.input.degrees.x = COM_CapAngleDegrees(motor->state.input.degrees.x);
				motor->state.input.degrees.y = info.yawRadians * RAD2DEG;
				//printf("Pitch: %.4f\n", motor->state.input.degrees.x);
			}
			
		} break;

        case AI_STATE_CHARGING:
        {
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            #if 1
            motor->state.input.degrees.x += -(600 * time->deltaTime);
            motor->state.input.degrees.x = COM_CapAngleDegrees(motor->state.input.degrees.x);
            #endif
            // Debugging 'Left/Right of'
            #if 0
            f32 yawDegrees = motor->state.input.degrees.y;
            f32 yawRadians = motor->state.input.degrees.y * DEG2RAD;
            // cos(x) sin(z) gives infront/behind...?
            // cos(z) sin(x) gives left of/right of...?
            f32 vx = cosf(yawRadians);
            f32 vz = sinf(yawRadians);
            //f32 vz = cosf(yawRadians);
            //f32 vx = sinf(yawRadians);

            //motor->state.input.degrees.y += 10 * time->deltaTime;
            //motor->state.input.degrees.y = 45;
            //motor->state.input.degrees.y = 135;
            //motor->state.input.degrees.y = 225;
            //motor->state.input.degrees.y = 270;
            motor->state.input.degrees.y = 180;
            motor->state.input.degrees.y = COM_CapAngleDegrees(motor->state.input.degrees.y);

            EC_Transform* selfTrans = EC_FindTransform(gs, &ai->header.entId);
            EC_Transform* tarTrans = EC_FindTransform(gs, &ai->state.target);
            u8 isLeft = IsPointLeftOfLine(
                selfTrans->t.pos.x, selfTrans->t.pos.z,
                vx, vz,
                tarTrans->t.pos.x, tarTrans->t.pos.z
            );
            if (isLeft)
            {
                printf("Facing %.2f degrees. vx/vz: %.2f/%.2f Left of\n", yawDegrees, vx, vz);
                motor->state.input.degrees.x += -(600 * time->deltaTime);
                motor->state.input.degrees.x = COM_CapAngleDegrees(motor->state.input.degrees.x);
            }
            else
            {
                printf("Facing %.2f degrees. vx/vz: %.2f/%.2f  Right of\n", yawDegrees, vx, vz);
                motor->state.input.degrees.x += -(600 * time->deltaTime);
                motor->state.input.degrees.x = COM_CapAngleDegrees(motor->state.input.degrees.x);
                //printf("Reduce think\n");
                //AI_ReduceNextThink(ai, time, 0.2f);
            }
            #endif
        } break;

        case AI_STATE_FINISH_ATTACK:
        {
            ai->state.state = AI_STATE_TRACKING;
            EC_ActorMotor* motor = EC_FindActorMotor(gs, &EC_GET_ID(ai));
            motor->state.input.buttons = 0;
        } break;
    }
    #endif
}

static void Game_UpdateAIControllers(ClientList* clients, GameScene *gs, GameTime *time)
{
    u32 max = gs->aiControllerList.max;
    for (u32 i = 0; i < max; ++i)
    {
        EC_AIController *ai = &gs->aiControllerList.items[i];
        if (ai->header.inUse == 0)
        {
            continue;
        }
        Ent* self = Ent_GetEntityById(&gs->entList, &ai->header.entId);
        EC_AIState *s = &ai->state;
        AI_Tick(clients, gs, self, ai, time);
    }
}
