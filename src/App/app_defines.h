#pragma once

#define MAX_INPUT_ITEMS 32

#define GAME_MAX_CLIENTS 8

#define APP_MAX_MENU_ITEMS 256
#define APP_FIRST_MAP "maps\\testbox.lvl"

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
