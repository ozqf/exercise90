#pragma once

#include <stdint.h>

/*****************************************************
PRIMITIVE TYPES
*****************************************************/
#define f32 float
#define f64 double

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define u8 uint8_t
#define uChar uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define bool32 uint32_t

static_assert(sizeof(i8) == 1, "Code requires i8 size == 1");
static_assert(sizeof(u8) == 1, "Code requires u8 size == 1");
static_assert(sizeof(i16) == 2, "Code requires i16 size == 2");
static_assert(sizeof(u16) == 2, "Code requires u16 size == 2");
static_assert(sizeof(i32) == 4, "Code requires int size == 4");
static_assert(sizeof(u32) == 4, "Code requires uint size == 4");

static_assert(sizeof(f32) == 4, "Code requires f32 size == 4");
static_assert(sizeof(f64) == 8, "Code requires f64 size == 8");

/*****************************************************
MACROS
*****************************************************/
#define internal static
#define local_persist static 
#define global_variable static

// Quick cross platform assert. Read from address zero if expression is false
// TODO: Message box assert with a print of __FILE__, __LINE__ and __TIME__ possible?
#if PARANOID
#define Assert(expression) if(!(expression)) {*(int *)0 = 0; }
#else
#define Assert(expression)
#endif
#define AssertAlways(expression) if(!(expression)) { *(int *)0 = 0; }

#define ILLEGAL_CODE_PATH if (true) { *(int *)0 = 0; }

#define KiloBytes(bytes) ((bytes) * 1024LL)
#define MegaBytes(bytes) (KiloBytes(bytes) * 1024LL)

#define F32_EPSILON 1.19209290E-07F // decimal constant

#define DLL_EXPORT __declspec(dllexport)

#define COM_ERROR_NONE 0

#define COM_ERROR_BAD_INDEX -1
#define COM_ERROR_UNKNOWN 1
#define COM_ERROR_UNSUPPORTED_OPTION 2
#define COM_ERROR_OPEN_SOCKET_FAILED 3
#define COM_ERROR_MISSING_FILE 4

///////////////////////////////////////////////////////////////////////
// Buffer macros
///////////////////////////////////////////////////////////////////////

// I hate writing casts all the time okay?
// returns amount of bytes copied. so you can do
// readPosition += COM_COPY(readPosition, writePosition, sizeof(SomeStruct));
#ifndef COM_COPY
#define COM_COPY(ptrSource, ptrDestination, numBytes) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, numBytes##)
#endif

// copy but automatically do a size of
#ifndef COM_COPY_STRUCT
#define COM_COPY_STRUCT(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##))
#endif

#ifndef COM_COMPARE
#define COM_COMPARE(ptrA, ptrB, numBytes) \
COM_CompareMemory((u8*)##ptrA##, (u8*)##ptrB##, numBytes##)
#endif
