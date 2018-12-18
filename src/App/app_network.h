#pragma once

/**
 * Notes:
 * Handles callbacks from ZNet connection layer and 
 * the client/server command streams that go into/out of the
 * game state
 */


void Net_TransmitSay(GameSession* session, char* str, char** tokens, i32 numTokens)
void NET_WriteImpulse(GameSession* gs, i32 impulse);
ZNetPlatformFunctions Net_CreateNetworkPlatformFunctions();
ZNetOutputInterface Net_CreateOutputInterface();
