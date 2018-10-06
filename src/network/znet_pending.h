#pragma once

#include "znet_module.cpp"

ZNetPending* ZNet_FindPendingConnection(ZNet* net, i32 xor)
{
	ZNetPending* p = NULL;
	for (i32 i = 0; i < MAX_PENDING_CONNECTIONS; ++i)
	{
		p = &net->pendingConnections[i];
		i32 pXor = p->clientSalt ^ p->challenge;
		printf("XOR %d vs %d\n", xor, pXor);
		if (pXor == xor)
		{
			return p;
		}
	}
	return NULL;
}

/*internal*/ ZNetPending* ZNet_AddPendingConnection(ZNet* net, ZNetAddress* address, u32 clientSalt)
{
    i32 firstFree = -1;
	ZNetPending* p = NULL;
    for (i32 i = 0; i < MAX_PENDING_CONNECTIONS; ++i)
    {
		p = &net->pendingConnections[i];
        //if (net->pendingConnections[i].)
        if (p->challenge == 0)
        {
			if (firstFree == -1)
			{
				firstFree = i;
			}
        }
		else
		{
			if (p->clientSalt == clientSalt && COM_COMPARE(&p->address, address, sizeof(ZNetAddress)))
			{
				printf("SV Repeat request from %d\n", clientSalt);
				return p;
			}
		}
    }
	// Create new pending request
	if (firstFree != -1)
	{
		p = &net->pendingConnections[firstFree];
		p->challenge = ZNet_CreateSalt();
		p->clientSalt = clientSalt;
		p->address = *address;
		printf("SV Adding pending connection to \"%d.%d.%d.%d:%d\"\nClient Salt %d Challenge Salt %d\n",
			p->address.ip4Bytes[0],
			p->address.ip4Bytes[1],
			p->address.ip4Bytes[2],
			p->address.ip4Bytes[3],
			p->address.port,
			p->clientSalt,
			p->challenge
		);
	}
	else
	{
		printf("SV No free challenge slots\n");
	}
    return p;
}

void ZNet_ClosePendingConnection(ZNet* net, ZNetPending* p)
{
	p->challenge = 0;
}

void ZNet_CheckPendingConnections(ZNet* net)
{
	/*
	Iterate over pending connections list
	> If the request is old, free the slot
	> ...otherwise, resend responses? Or just send response per request packet?
	*/
	for (i32 i = 0; i < MAX_PENDING_CONNECTIONS; ++i)
	{
		ZNetPending* p = &net->pendingConnections[i];
		if (p->challenge == 0) { continue; }
		p->ticks++;
		if (p->ticks >= MAX_PENDING_CONNECTION_TICKS)
		{
			*p = {};
		}
	}
}

void ZNet_PrintPendingConnections(ZNet* net)
{
	printf("- Pending Connections -\n");
	for (i32 i = 0; i < MAX_PENDING_CONNECTIONS; ++i)
	{
		ZNetPending* p = &net->pendingConnections[i];
		i32 c = p->challenge;
		if (c)
		{
			printf("%d Active: %d\n", i, c);
		}
		else
		{
			printf("%d Open: %d\n", i, c);
		}
	}
	//printf("")
}
