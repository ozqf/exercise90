#pragma once

///////////////////////////////////////////////////////////////////////
// Error handling
///////////////////////////////////////////////////////////////////////
typedef int ErrorCode;

#define COM_ERROR_NONE 0

#define COM_ERROR_BAD_INDEX -1
#define COM_ERROR_UNKNOWN 1
#define COM_ERROR_UNSUPPORTED_OPTION 2
#define COM_ERROR_OPEN_SOCKET_FAILED 3
#define COM_ERROR_MISSING_FILE 4
#define COM_ERROR_UNKNOWN_COMMAND 5
#define COM_ERROR_NO_SPACE 6
#define COM_ERROR_SERIALISE_FAILED 7
#define COM_ERROR_DESERIALISE_FAILED 8
#define COM_ERROR_BAD_SIZE 9
#define COM_ERROR_NOT_FOUND 10
#define COM_ERROR_BAD_ARGUMENT 11
#define COM_ERROR_NULL_ARGUMENT 12
#define COM_ERROR_NOT_IMPLEMENTED 13
#define COM_ERROR_ALLOCATION_FAILED 14
#define COM_ERROR_OPERATION_FAILED 15
#define COM_ERROR_FUNC_RAN_AWAY 16

typedef void (*COM_FatalErrorFunction)(const char* message, const char* heading);

void COM_SetFatalError(COM_FatalErrorFunction func);

void COM_Fatal(char* msg);

#define COM_ASSERT(expression, msg) if(!(expression)) \
{ \
    char assertBuf[512]; \
    snprintf(assertBuf, 512, "%s, %d: %s\n", __FILE__, __LINE__, msg); \
	COM_Fatal(assertBuf); \
}
