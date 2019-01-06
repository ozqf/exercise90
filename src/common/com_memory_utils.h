#pragma once

#include "com_defines.h"

com_internal u32 SafeTruncateUInt64(u64 value);

////////////////////////////////////////////////////////////
// TODO: None of this handles eddianness!
// Implement via compile time flag, #define BIG_ENDIAN
////////////////////////////////////////////////////////////

/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteByte(u8 value, u8* target);
com_internal void COM_WriteByte(u8 value, u8** target);
/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteI16(i16 value, u8* target);
/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteU16(u16 value, u8* target);
/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteI32(i32 value, u8* target);
/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteU32(u32 value, u8* target);
/**
 * Returns number of bytes written
 */
com_internal u32 COM_WriteF32(f32 value, u8* target);
/**
 * Read an i32 at the target pointer position AND move the target pointer forward
 */
com_internal u8 COM_ReadByte(u8** target);
com_internal u16 COM_ReadU16(u8** target);
/**
 * Read an i32 at the target pointer position AND move the target pointer forward
 */
com_internal i32 COM_ReadI32(u8** target);
com_internal u32 COM_ReadU32(u8** target);
com_internal f32 COM_ReadF32(u8** target);
/**
 * Read an i32 at the target position, keeping target in place
 */
com_internal i32 COM_PeekI32(u8* target);
/**
 * Returns number of bytes written
 */
com_internal u32 COM_CopyMemory(u8* source, u8* target, u32 numBytes);
// returns 1 if two blocks of memory are identical. 0 if otherwise.
com_internal i32 COM_CompareMemory(u8* ptrA, u8* ptrB, u32 numBytes);
com_internal void COM_ZeroMemory(u8 *ptr, u32 numBytes);
com_internal void COM_SetMemory(u8 *ptr, u32 numBytes, u8 val);
com_internal u32 COM_GetI32Sentinel();
/**
 * Repeatedly write the given pattern into ptr
 * Pattern size must fit exactly into numBytes!
 */
com_internal void COM_SetMemoryPattern(u8* ptr, u32 numBytes, u8* pattern, u32 patternSize);
com_internal void COM_DebugFillMemory(u8 *ptr, u8 value, u32 numBytes);
/**
Returns <value> enlarged to match the alignment specified.
eg value = 52, align = 32 == 64, 
52 mod 32 = 20
32 - 20 = 12
52 + 12 = 64

but
64 mod 32 = 0
32 - 0 = 32
64 + 32 = 96

*/
com_internal u32 COM_AlignSize(u32 value, u32 alignment);
com_internal i32 COM_SimpleHash(u8* ptr, i32 numBytes);