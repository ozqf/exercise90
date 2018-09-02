#pragma once

#include "game.h"
#include "../common/com_module.h"

#include "EntityComponents/ec_actor.h"
#include "EntityComponents/ec_collider.h"
#include "EntityComponents/ec_thinker.h"

Ent *Game_FindEntityByLabel(GameState *gs, char *queryLabel)
{
    for (u32 i = 0; i < gs->labelList.max; ++i)
    {
        EC_Label *entLabel = &gs->labelList.items[i];
        if (!COM_CompareStrings(entLabel->state.label, queryLabel))
        {
            return Ent_GetEntityById(&gs->entList, &entLabel->header.entId);
        }
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////
// Projectiles
///////////////////////////////////////////////////////////////////
inline void Prj_PushRigidBody(EC_Collider *col)
{
    PhysCmd_ChangeVelocity(col->shapeId, 0.0f, 10.0f, 0.0f);
}

void Game_UpdateProjectiles(GameState *gs, GameTime *time)
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
            t->pos.z + prj->state.move.z * time->deltaTime
        };

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
                if (health != NULL)
                {
                    health->state.hp -= 10;
                    // kill victim
                    if (health->state.hp <= 0)
                    {
                        Ent_WriteRemoveCmd(
                            targetEnt,
                            hit->normal,
                            time->singleFrame == 1);
                        // is this the time to do this...?
                        // TODO ^^ No, it isn't!
                        PhysCmd_RemoveShape(col->shapeId);
                        //printf("Killed cube!\n");
                        Vec3 dir = { prj->state.move.x, prj->state.move.y, prj->state.move.z };
                        Vec3_Normalise(&dir);
                        for (i32 k = 0; k < 10; ++k)
                        {
                            Game_SpawnLocalEntity(hit->worldPos[0], hit->worldPos[1], hit->worldPos[2], &dir, 10, LOCAL_ENT_TYPE_DEBRIS);
                        }
                    }
                    else
                    {
                        //printf("Hit cube, hp: %d\n", health->hp);
                        EC_AIController* ai = EC_FindAIController(gs, &targetEnt->entId);
                        if (ai != NULL)
                        {
                            AI_Stun(gs, ai, time);
                        }
                        //Prj_PushRigidBody(col);
                    }
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

void Game_UpdateUI(UIEntity *ents, i32 maxEntities, GameTime *time)
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
