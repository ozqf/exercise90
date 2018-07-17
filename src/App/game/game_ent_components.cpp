#pragma once

#include "game.h"
#include "../common/com_module.h"

#include "EntityComponents/ec_actor.h"
#include "EntityComponents/ec_collider.h"
#include "EntityComponents/ec_thinker.h"

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

///////////////////////////////////////////////////////////////////
// Projectiles
///////////////////////////////////////////////////////////////////
inline void Prj_PushRigidBody(EC_Collider* col)
{
    Phys_ChangeVelocity(col->shapeId, 0.0f, 10.0f, 0.0f);
}

void Game_UpdateProjectiles(GameState* gs, GameTime* time)
{
    for (u32 i = 0; i < gs->projectileList.max; ++i)
    {
        EC_Projectile* prj = &gs->projectileList.items[i];
        if (prj->inUse == 0) { continue; }

        Ent* e = Ent_GetEntityByIndex(&gs->entList, prj->entId.index);

        if (prj->ticker.tick <= 0.0f)
        {
            // Delete
            Vec3 v = Vec3_CreateUnitVector(&prj->move);
            Ent_WriteRemoveCmd(e, v.parts, time->singleFrame == 1);
            #if 0
            Ent_MarkForFree(e);
            Cmd_RemoveEntity cmd = {};
            cmd.entId = e->entId;
			if (time->singleFrame) { printf("GAME Delete prj %d/%d\n",
				cmd.entId.iteration,
				cmd.entId.index);
			}
            App_WriteGameCmd((u8*)&cmd, CMD_TYPE_REMOVE_ENT, sizeof(Cmd_RemoveEntity));
            #endif
            //Ent_Free(gs, e);
        }
        else
        {
            prj->ticker.tick -= time->deltaTime;
        }

        Transform* t = &e->transform;

        Vec3 newPos =
        {
            t->pos.x + prj->move.x * time->deltaTime,
            t->pos.y + prj->move.y * time->deltaTime,
            t->pos.z + prj->move.z * time->deltaTime
        };

        //Phys_RayTest(t->pos.x, t->pos.y, t->pos.z, newPos.x, newPos.y, newPos.z);
        PhysRayHit results[12];
        
        PhysCmd_Raycast ray = {};
        ray.start[0] = t->pos.x;
        ray.start[1] = t->pos.y;
        ray.start[2] = t->pos.z;

        ray.end[0] = newPos.x;
        ray.end[1] = newPos.y;
        ray.end[2] = newPos.z;

        i32 numHits = Phys_QueryRay(&ray, results, 12);

        u8 survived = 1;

        if (numHits > 0)
        {
            for (i32 j = 0; j < numHits; ++j)
            {
                PhysRayHit* hit = &results[j];
                EC_Collider* col = EC_ColliderGetByShapeId(&gs->colliderList, results[j].shapeId);
                Assert(col != NULL);
                Ent* targetEnt = Ent_GetEntityById(&gs->entList, &col->entId);
				if (targetEnt == NULL)
				{
					printf("!GAME prj victim %d/%d is NULL!\n", col->entId.iteration, col->entId.index);
					continue;
				}
                if (targetEnt->factoryType == ENTITY_TYPE_RIGIDBODY_CUBE)
                {
                    EC_Health* health = EC_FindHealth(gs, &col->entId);
                    if (health != NULL)
                    {
                        health->hp -= 10;
                        // kill victim
                        if (health->hp <= 0)
                        {
                            Ent_WriteRemoveCmd(
                                targetEnt,
                                hit->normal,
                                time->singleFrame == 1
                            );
                            // is this the time to do this...?
                            Phys_RemoveShape(col->shapeId);
                            //printf("Killed cube!\n");
                        }
                        else
                        {
                            //printf("Hit cube, hp: %d\n", health->hp);
                            Prj_PushRigidBody(col);
                        }
                    }
                    
                    // kill prj
                    t->pos.x = hit->worldPos[0];
                    t->pos.y = hit->worldPos[1];
                    t->pos.z = hit->worldPos[2];
                    survived = 0;
                    Ent_WriteRemoveCmd(e, hit->normal, time->singleFrame == 1);
                }
                else if (targetEnt->factoryType == ENTITY_TYPE_WORLD_CUBE)
                {
                    // kill prj
                    t->pos.x = hit->worldPos[0];
                    t->pos.y = hit->worldPos[1];
                    t->pos.z = hit->worldPos[2];
                    survived = 0;
                    Ent_WriteRemoveCmd(e, hit->normal, time->singleFrame == 1);
                }
            }
        }
        if (survived)
        {
            // Move
            t->pos.x += prj->move.x * time->deltaTime;
            t->pos.y += prj->move.y * time->deltaTime;
            t->pos.z += prj->move.z * time->deltaTime;
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
