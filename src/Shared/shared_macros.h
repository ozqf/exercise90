#pragma once


///////////////////////////////////////////////////////////////////////
// Macros
///////////////////////////////////////////////////////////////////////
#ifndef COM_COPY
#define COM_COPY(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##))
#endif

#ifndef COM_COPY_STEP
#define COM_COPY_STEP(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##)); \
ptrDestination = (u8*)((u32)ptrDestination + (u32)sizeof(##structType##))
#endif

/*
Single line for if case matches n, read a struct from a buffer and call a function with it
*/
#ifndef COM_HANDLE_EVENT_CASE
#define COM_HANDLE_EVENT_CASE(eventTypeInteger, structType, ptrBytes, ptrBufferStart, bufferSize, functionName) \
case eventTypeInteger##: \
{ \
	ASSERT(COM_CheckBufferHasSpace((u8*)##ptrBytes##, (u8*)ptrBufferStart##, bufferSize##, sizeof(##structType##))); \
    structType ev = {}; \
    COM_COPY(##ptrBytes##, &##ev##, structType##); \
    ptrBytes = ((u8*)##ptrBytes + sizeof(##structType##)); \
} break;
#endif
