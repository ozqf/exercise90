#pragma once

#include "game.h"
#include "../common/com_module.h"

#include "EntityComponents/ec_actor.h"
#include "EntityComponents/ec_collider.h"
#include "EntityComponents/ec_thinker.h"

/*internal*/ EntId Game_FindEntIdByLabel(GameState *gs, char *queryLabel)
{
    for (u32 i = 0; i < gs->labelList.max; ++i)
    {
        EC_Label *entLabel = &gs->labelList.items[i];
        if (!COM_CompareStrings(entLabel->state.label, queryLabel))
        {
            return entLabel->header.entId;
        }
    }
    return {};
}

///////////////////////////////////////////////////////////////////
// Render Objects
///////////////////////////////////////////////////////////////////

internal void Game_UpdateRenderObjects(GameState *gs, GameTime *time)
{
    for (u32 i = 0; i < gs->singleRendObjList.max; ++i)
    {
        EC_SingleRendObj *obj = &gs->singleRendObjList.items[i];
        if (obj->header.inUse == 0)
        {
            continue;
        }
		if (obj->state.flashFrames > 0)
		{
			obj->rendObj.SetColour(1, 1, 1);
			obj->state.flashFrames--;
		}
		else
		{
			f32* col = obj->state.colourRGB;
			obj->rendObj.SetColour(col[0], col[1], col[2]);
		}
	}
    for (u32 i = 0; i < gs->multiRendObjList.max; ++i)
    {
        EC_MultiRendObj *obj = &gs->multiRendObjList.items[i];
        if (obj->header.inUse == 0)
        {
            continue;
        }
		if (obj->state.objStates[0].flashFrames > 0)
		{
			obj->rendObjs[0].SetColour(1, 1, 1);
			obj->rendObjs[1].SetColour(1, 1, 1);
			obj->state.objStates[0].flashFrames--;
		}
        else
		{
			f32* col0 = obj->state.objStates[0].colourRGB;
			f32* col1 = obj->state.objStates[1].colourRGB;
			obj->rendObjs[0].SetColour(col0[0], col0[1], col0[2]);
			obj->rendObjs[1].SetColour(col1[0], col1[1], col1[2]);
		}
	}
}

///////////////////////////////////////////////////////////////////
// Projectiles
///////////////////////////////////////////////////////////////////
inline void Prj_PushRigidBody(EC_Collider *col)
{
    PhysCmd_ChangeVelocity(col->shapeId, 0.0f, 10.0f, 0.0f);
}

internal void Game_UpdateProjectiles(GameState *gs, GameTime *time)
{
    for (u32 i = 0; i < gs->projectileList.max; ++i)
    {
        EC_Projectile *prj = &gs->projectileList.items[i];
        if (prj->header.inUse == 0)
        {
            continue;
        }

        Ent *e = Ent_GetEntityById(&gs->entList, &prj->header.entId);
        if (e == NULL)
        {
            printf("ERROR: Prj has no Ent\n");
            ILLEGAL_CODE_PATH
        }

        EC_Transform *ecTrans = EC_FindTransform(gs, &prj->header.entId);

        if (prj->state.ticker.tick <= 0.0f)
        {
            // Delete
            Vec3 v = Vec3_CreateUnitVector(&prj->state.move);
            Ent_WriteRemoveCmd(e, v.parts, time->singleFrame == 1);
        }
        else
        {
            prj->state.ticker.tick -= time->deltaTime;
        }

        Transform *t = &ecTrans->t;

        Vec3 newPos =
            {
                t->pos.x + prj->state.move.x * time->deltaTime,
                t->pos.y + prj->state.move.y * time->deltaTime,
                t->pos.z + prj->state.move.z * time->deltaTime};

        PhysRayHit results[12];

        PhysCmd_Raycast ray = {};
        ray.mask = COL_MASK_PROJECTILE;
        ray.group = COLLISION_LAYER_ACTOR;
        ray.start[0] = t->pos.x;
        ray.start[1] = t->pos.y;
        ray.start[2] = t->pos.z;

        ray.end[0] = newPos.x;
        ray.end[1] = newPos.y;
        ray.end[2] = newPos.z;

        i32 numHits = PhysExt_QueryRay(&ray, results, 12);

        u8 survived = 1;

        if (numHits > 0)
        {
            for (i32 j = 0; j < numHits; ++j)
            {
                PhysRayHit *hit = &results[j];
                EC_Collider *col = EC_ColliderGetByShapeId(&gs->colliderList, results[j].shapeId);

                Assert(col != NULL);
                Ent *targetEnt = Ent_GetEntityById(&gs->entList, &col->header.entId);
                if (targetEnt == NULL)
                {
                    // If an entity was marked for deletion by a previous projectile/attack this frame
                    // it can be detected but is already dead so ignore
                    //printf("!GAME prj victim %d/%d is NULL!\n", col->header.entId.iteration, col->header.entId.index);
                    continue;
                }
                if (targetEnt->entId.value == e->source.value)
                {
                    if (gs->verbose)
                    {
                        printf("PRJ from %d hit it's source ent %d\n", e->source.value, targetEnt->entId.value);
                    }
                    continue;
                }
                if (gs->verbose)
                {
                    printf("PRJ from %d hit ent %d\n", e->source.value, targetEnt->entId.value);
                }

                // Attempt to deal damage
                EC_Health *health = EC_FindHealth(gs, &col->header.entId);
                i32 dmg = 10;
                if (health != NULL && Game_AttackIsValid(e->team, targetEnt->team))
                {
                    health->state.hp -= dmg;
                    health->state.damageThisFrame += dmg;

                    health->state.lastHitFrame = time->gameFrameNumber;
                    health->state.lastHitSource = e->source;
                    #if 0
                    if (health->state.flags & EC_STATE_FLAG_IS_PLAYER)
                    {
                        EC_Transform* sourceTransform = EC_FindTransform(gs, &e->source);
                        if (sourceTransform)
                        {
                            Cmd_SpawnHudItem cmd = {};
                            cmd.pos.x = sourceTransform->t.pos.x;
                            cmd.pos.y = sourceTransform->t.pos.y;
                            cmd.pos.z = sourceTransform->t.pos.z;
                            cmd.source = e->source;
                            cmd.victim = health->header.entId;
                            APP_WRITE_CMD(0, CMD_TYPE_SPAWN_HUD_ITEM, 0, cmd);
                        }
                    }
                    #endif
                }

                // kill prj
                t->pos.x = hit->worldPos[0];
                t->pos.y = hit->worldPos[1];
                t->pos.z = hit->worldPos[2];
                survived = 0;
                Ent_WriteRemoveCmd(e, hit->normal, time->singleFrame == 1);
            }
        }
        if (survived)
        {
            // Move
            t->pos.x += prj->state.move.x * time->deltaTime;
            t->pos.y += prj->state.move.y * time->deltaTime;
            t->pos.z += prj->state.move.z * time->deltaTime;
        }
    }
}

/////////////////////////////////////////////////////////////////////////
// HEALTH
/////////////////////////////////////////////////////////////////////////
internal void Game_UpdateHealth(GameState *gs, GameTime *time)
{
    for (u32 i = 0; i < gs->healthList.max; ++i)
    {
        EC_Health *health = &gs->healthList.items[i];
        if (health->header.inUse == 0)
        {
            continue;
        }
		
		EntId* selfId = &health->header.entId;

        if (health->state.hp <= 0)
        {
            if (health->state.flags & EC_HEALTH_FLAG_INVULNERABLE)
            {
                health->state.hp = 1;
            }
            else
            {
                f32 normal[3] = {0, 1, 0};
                Ent_WriteRemoveCmd(
                    Ent_GetEntityById(gs, &health->header.entId),
                    normal,
                    time->singleFrame == 1);

                /**********************************
             * // is this the time to do this...?
            // TODO ^^ No, it isn't!
            **********************************/
                EC_Collider *col = EC_FindCollider(gs, &health->header.entId);
                Assert(col);
                PhysCmd_RemoveShape(col->shapeId);

                //Game_SpawnLocalEntity(hit->worldPos[0], hit->worldPos[1], hit->worldPos[2], NULL, 0, LOCAL_ENT_TYPE_EXPLOSION
                //printf("Killed cube!\n");
                //Vec3 dir = { prj->state.move.x, prj->state.move.y, prj->state.move.z };
                Vec3 dir = {normal[0], normal[1], normal[2]};
                Vec3_Normalise(&dir);

                EC_Transform *trans = EC_FindTransform(gs, &health->header.entId);
                Transform *t = &trans->t;
                Assert(trans);

                f32 launchPower = 20.0f; // * percentage;
                if (health->state.maxHp > 0)
                {
                    i32 overkill = -health->state.hp;
                    f32 percentage = ((f32)overkill / (f32)health->state.maxHp);
                    launchPower = 20.0f + (20.0f * percentage);
                }

                for (i32 k = 0; k < 10; ++k)
                {
                    //printf("Launch dir %.2f %.2f %.2f\n", dir.x, dir.y, dir.z);
                    Vec3 offset = Game_RandomSpawnOffset(0.5f, 1.0f, 0.5f);
                    f32 power = 10.0f + Game_RandomInRange(launchPower);
                    Game_SpawnLocalEntity(
                        t->pos.x + offset.x, t->pos.y + offset.y, t->pos.z + offset.z,
                        &dir, power, LOCAL_ENT_TYPE_DEBRIS);
                }
            }
        }
        else if (health->state.lastHitFrame == time->gameFrameNumber)
        {
            #if 1
            if (health->state.flags & EC_STATE_FLAG_IS_PLAYER)
            {
                Client* cl = App_FindLocalClient(&gs->clientList, 1);
                if (health->header.entId.value == cl->entId.value)
                {
                    printf("Spawn hud ring\n");
                    // Spawn hit gfx
                    Cmd_SpawnHudItem cmd = {};
                    EC_Transform* attackerXForm = EC_FindTransform(gs, &health->state.lastHitSource);
                    if (attackerXForm)
                    {
                        cmd.pos = attackerXForm->t.pos;
                        cmd.source = health->state.lastHitSource;
                        cmd.victim = health->header.entId;
                        APP_WRITE_CMD(0, CMD_TYPE_SPAWN_HUD_ITEM, 0, cmd);
                    }
                    else
                    {
                        printf("Attacked has no transform!\n");
                    }
                }
            }
            #endif
			EC_MultiRendObj* multiObj = EC_FindMultiRendObj(gs, selfId);
			if (multiObj)
			{
				multiObj->state.objStates[0].flashFrames = 3;
			}
			EC_SingleRendObj* singleRend = EC_FindSingleRendObj(gs, selfId);
			if (singleRend)
			{
				singleRend->state.flashFrames = 3;
			}
            if (health->state.damageThisFrame > health->state.stunThreshold)
            {
                //printf("Hit cube, hp: %d\n", health->hp);
                EC_AIController *ai = EC_FindAIController(gs, &health->header.entId);
                if (ai != NULL)
                {
                    AI_Stun(gs, ai, time);
                }
                //Prj_PushRigidBody(col);
            }
        }
        

        health->state.damageThisFrame = 0;
    }
}

internal void EC_SensorHandleHit(
    GameState* gs,
    GameTime* time,
    EC_Sensor* sensor,
    Ent* self,
    Ent* target,
    i32 collisionState)
{
    if (collisionState == 3) { return; }
    if (collisionState == 2)
    {
        printf("OVERLAP\n");
        return;
    }
    if (sensor->state.damageSourceComponent & EC_FLAG_AICONTROLLER)
    {
        if (!Game_AttackIsValid(self->team, target->team)) { return; }
        EC_AIController* ai = EC_FindAIController(gs, &sensor->header.entId);
        Assert(ai);
        if (ai->state.state != AI_STATE_CHARGING) { return; }
        EC_Health* health = EC_FindHealth(gs, target);
        if (!health) { return; }
        health->state.hp -= 10;
        health->state.damageThisFrame += 10;
        // Add one to the recorded frame as it will be incremented before health reads it
		health->state.lastHitFrame = time->gameFrameNumber + 1;
		health->state.lastHitSource = self->entId;
    }
    if (sensor->state.damageSourceComponent & EC_FLAG_VOLUME)
    {
        EC_Volume* vol = EC_FindVolume(gs, &sensor->header.entId);
        Assert(vol);
        
    }
}

internal void Game_UpdateSensors(GameState* gs, GameTime* time)
{

}
