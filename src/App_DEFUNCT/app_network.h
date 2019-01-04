#pragma once

/**
 * Notes:
 * Handles callbacks from ZNet connection layer and 
 * the client/server command streams that go into/out of the
 * game state
 */

ZNetOutputInterface     Net_CreateOutputInterface();
void                    Net_TransmitSay(GameSession* session, char* str, char** tokens, i32 numTokens)
void                    NET_WriteImpulse(GameSession* gs, i32 impulse);
void                    Net_ReadPackets(GameSession* session, GameScene* gs, GameTime* time);
void                    Net_ReadInputStreams(GameSession* session, GameScene* gs, GameTime* time);
void                    Net_Transmit(GameSession* session, GameScene* gs, GameTime* time);
