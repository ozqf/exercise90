#pragma once

#include "game.h"

// Enable replication of entities for the given client.
void SV_BeginEntityReplication(Client *cl, GameScene *gs)
{
    // set flag, send a current entity snapshot
    printf("SV Begin entity replication to conn %d\n", cl->connectionId);
    
    ByteBuffer* output = &cl->stream.outputBuffer;

    u8 temp[MAX_ENTITY_STATE_CMD_SIZE];
    #if 1
    i32 entsWritten = 0;
    // scene sync
    for (i32 i = 0; i < ENTITY_BLOCK_SIZE; ++i)
    {
        Ent* ent = &gs->entList.items[i];
        if (ent->inUse == ENTITY_STATUS_IN_USE)
        {
            EntityState es;
            Ent_CopyFullEntityState(gs, ent, &es);
            u16 stateSize = Ent_WriteStateToBuffer(&es, temp, MAX_ENTITY_STATE_CMD_SIZE);
            u32 msgId = ++cl->stream.outputSequence;
            output->ptrWrite += Stream_WriteStreamMsgHeader(
                output->ptrWrite, msgId, stateSize, NET_DEFAULT_RESEND_RATE);
            output->ptrWrite += COM_COPY(temp, output->ptrWrite, stateSize);
            entsWritten++;
        }
    }
    
    cl->syncCompleteMessageId = cl->stream.outputSequence - 1;
    printf("SV Wrote %d ent states for CL %d output space %d\n",
        entsWritten, cl->connectionId, output->Space());
    if (entsWritten > 0)
    {
        Stream_PrintBufferManifest(output);
    }
    printf("  Sync completion seq: %d\n", cl->syncCompleteMessageId);


    //NET_MSG_TO_OUTPUT(&cl->stream, &cmd);
    #endif
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
        b->ptrWrite += Stream_WriteStreamMsgHeader(b->ptrWrite, msgId, header->GetSize(), NET_DEFAULT_RESEND_RATE);
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
