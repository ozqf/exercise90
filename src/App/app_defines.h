#pragma once

#define MAX_INPUT_ITEMS 32

#define GAME_MAX_CLIENTS 8

#define APP_MAX_MENU_ITEMS 256
//#define APP_FIRST_MAP "maps\\testbox.lvl"
#define APP_FIRST_MAP "TEST"

#ifndef APP_WRITE_CMD
#define APP_WRITE_CMD(i32_bufferId, u8_cmdType, u8_cmdFlags, cmdObject) \
{ \
	GameTime* ptrGameTime = GetAppTime(); \
	if (ptrGameTime->singleFrame)\
	{\
		char* label = App_GetBufferName(g_appWriteBuffer->ptrStart); \
		printf("Writing type %d to %s\n", u8_cmdType, label);\
	}\
\
    u8** ptrToWritePtr = &g_appWriteBuffer->ptrWrite; \
    COM_WRITE_CMD_TO_BUFFER(ptrToWritePtr, u8_cmdType, u8_cmdFlags, cmdObject) \
}
#endif

/////////////////////////////////////////////////////////////////
// Network message struct to output stream macro
/////////////////////////////////////////////////////////////////
#ifndef NET_MSG_TO_OUTPUT
#define NET_MSG_TO_OUTPUT(ptrNetStream, ptrMsgStruct) \
{ \
    u32 msgId = ++##ptrNetStream##->outputSequence; \
	u16 msgSize = ptrMsgStruct##->MeasureForWriting(); \
	ByteBuffer* ptrOutputBuf = &ptrNetStream->outputBuffer; \
	ptrOutputBuf->ptrWrite += Stream_WriteStreamMsgHeader(ptrOutputBuf->ptrWrite, msgId, msgSize); \
	ptrOutputBuf->ptrWrite += ptrMsgStruct##->Write(ptrOutputBuf->ptrWrite); \
    \
} \

#endif

#ifndef NET_MSG_TRANSMIT_TO_ALL_CLIENTS
#define NET_MSG_TRANSMIT_TO_ALL_CLIENTS(ptrClientList, ptrMsgStruct) \
{ \
	for (i32 iterator = 0; iterator < ptrClientList##->max; ++iterator) \
	{ \
		Client* ptrToClient = &##ptrClientList##->items[iterator]; \
		if (ptrToClient->state == CLIENT_STATE_FREE) { continue; } \
		NET_MSG_TO_OUTPUT(ptrToClient->stream, ptrMsgStruct##) \
	} \
}

#endif
