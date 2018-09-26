#pragma once

#include "znet_module.cpp"

/*internal*/ ZNetPending* ZNet_AddPendingConnection(ZNet* net, ZNetAddress* address, u32 clientSalt)
{
    i32 firstFree = -1;
    for (i32 i = 0; i < MAX_PENDING_CONNECTIONS; ++i)
    {
		ZNetPending* p = &net->pendingConnections[i];
        //if (net->pendingConnections[i].)
        if (p->challenge == 0)
        {
			if (i == -1)
			{
				firstFree = i;
			}
        }
		else
		{
			if (p->clientSalt == clientSalt && COM_COMPARE(&p->address, &address, sizeof(ZNetAddress)))
			{
				return p;
			}
		}
    }
    return NULL;
}
