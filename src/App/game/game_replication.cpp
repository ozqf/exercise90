#pragma once

#include "game.h"

// Enable replication of entities for the given client.
void SV_BeginEntityReplication(Client *cl, GameScene *scene)
{
    // set flag, send a current entity snapshot
}

void SV_ReplicateEntitySpawn(CmdHeader* header, ClientList *cls, u8 *read)
{
    APP_ASSERT(IS_SERVER, "Client trying to call Entity spawn replication");
    // TODO:: Entity state doesn't work directly through a command object
    // and requires this stuff to send it instead...
    for (i32 i = 0; i < cls->max; ++i)
    {
        Client *cl = &cls->items[i];
        // check in use or local
        if (cl->state == CLIENT_STATE_FREE)
        {
            continue;
        }
        // ...Actually, sen to client
        //if (cl->connectionId == 0) { continue; }

        NetStream *s = &cl->stream;
        ByteBuffer *b = &s->outputBuffer;
        u32 msgId = ++s->outputSequence;
        i32 requiredSpace = header->GetSize() + sizeof(StreamMsgHeader);
        i32 space = b->Space();
        if (space < requiredSpace)
        {
            printf("CLIENT %d has no output capacity (has %d need %d, written %d)!\n",
                   cl->connectionId, b->Space(), requiredSpace, b->Written());
            
            // Remote client and No output capacity?
            // TODO: Drop client!
            APP_ASSERT(false, "Oi dropping clients is not implemented!");
            continue;
        }
        //printf("SV Write state of ent to %d\n", cl->connectionId);
        b->ptrWrite += Stream_WriteStreamMsgHeader(b->ptrWrite, msgId, header->GetSize(), 0.1f);
        b->ptrWrite += COM_COPY(read, b->ptrWrite, header->GetSize());
    }
}

void SV_TickEntityReplication(
    GameSession *session,
    GameScene *scene,
    GameTime *time)
{
    for (i32 i = 0; i < session->clientList.max; ++i)
    {
        Client *cl = &session->clientList.items[i];
        if (cl->state == CLIENT_STATE_FREE)
        {
            continue;
        }
    }
}
