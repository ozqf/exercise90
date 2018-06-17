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

///////////////////////////////////////////////////////////////////////
// Buffer macros
///////////////////////////////////////////////////////////////////////

// I hate writing casts all the time okay?
#ifndef COM_COPY
#define COM_COPY(ptrSource, ptrDestination, numBytes) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, numBytes##)
#endif

// copy but automatically do a size of
#ifndef COM_COPY_STRUCT
#define COM_COPY_STRUCT(ptrSource, ptrDestination, structType) \
COM_CopyMemory((u8*)##ptrSource##, (u8*)##ptrDestination##, sizeof(##structType##))
#endif
