#pragma once

#include "com_module.h"


inline u32 SafeTruncateUInt64(u64 value);

void COM_CopyMemory(u8* source, u8* target, u32 numBytes);
void COM_ZeroMemory(u8 *ptr, u32 numBytes);
void COM_SetMemory(u8 *ptr, u32 numBytes, u8 val);
/**
 * Repeatedly write the given pattern into ptr
 * Pattern size must fit exactly into numBytes!
 */
void COM_SetMemoryPattern(u8* ptr, u32 numBytes, u8* pattern, u32 patternSize);
void COM_DebugFillMemory(u8 *ptr, u8 value, u32 numBytes);
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
u32 COM_AlignSize(u32 value, u32 alignment);

/**
 * if strings are equal, return 0
 * if a is first alphabetically, return -1
 * if b is first alphabetically, return 1
 */

i32 COM_CompareStrings(const char *a, const char *b);
void COM_CopyStringA(const char *source, char *target);
void COM_CopyString(const char *source, char *target);
/**
 * Copy a string without exceeding the specified limit
 * Limit does NOT include NULL terminator
 */
void COM_CopyStringLimited(const char *source, char *target, i32 limit);
/**
 * No checking whether source/target will clash
 * No checking that target has enough room!
 */
void COM_CopyStringCount(const char *source, char *target, i32 count);
i32 COM_StrLenA(const char* str);
i32 COM_StrLen(const char* str);
// decimal or hexadecimal
// negative and positive
// "-54" "12" "0x432146fd" "-0X4AbdC"
i32 COM_AsciToInt32(const char *str);
f32 COM_AsciToFloat32(const char *str);