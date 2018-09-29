#pragma once

#include "znet_module.cpp"

/*
General structure:

-- Connection Request --
- Header -
Type - Byte
ClientSalt
ServerSalt

- Header -
Type - Byte
ClientSalt
ServerSalt

Data

*/

// Client2Server
struct ZNet_ConnectionRequest
{
	u32 clientSalt;
};

// Server2Client
struct ZNet_ConnectionChallenge
{
	// the XOR of these two values HAS to be unique for client (potential or not).
	// instead of asking the client to reroll their salt, the server should reroll
	// it's salt until the XOR is unique.
	u32 clientSalt;
	u32 serverSalt;
};

// Client2Server
struct ZNet_ChallengeResponse
{
	u32 saltXOR;
};

struct ZNet_ConnectionAccepted
{
	u32 saltXOR;
};

struct ZNet_StandardPacketHeader_Thing_This_Name_Should_Be_Replaced
{
	u32 saltXOR;
};
