#pragma once

#include "com_defines.h"

static inline i32   COM_CompareStrings(const char *a, const char *b);
static inline i32   COM_MatchStringStart(const char *str, const char *start);
static inline void  COM_CopyStringA(const char *source, char *target);
static inline void  COM_CopyString(const char *source, char *target);
static inline i32   COM_CopyStringLimited(const char *source, char *target, i32 limit);
static inline void  COM_CopyStringCount(const char *source, char *target, i32 count);
static inline i32   COM_StrLenA(const char* str);
static inline i32   COM_StrLen(const char* str);
static inline i32   COM_StrReplace(char* str, char target, char replacement);
static inline void  COM_StrToUpperCase(char* str);
static inline void  COM_StrToLowerCase(char* str);
static inline u8    COM_CharIsDecimalInt32(const char c);
static inline u8    COM_AsciIsDecimalInt32(const char *str);
static inline i32   COM_AsciToInt32(const char *str);
static inline f32   COM_AsciToFloat32(const char *str);
static inline i32   COM_StripTrailingInteger(char* text, char separator, i32 failureValue);
static inline u8    COM_CheckForFileExtension(const char* filePath, const char* extension);
