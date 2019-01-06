#pragma once

#include "com_defines.h"

com_internal i32   COM_CompareStrings(const char *a, const char *b);
com_internal i32   COM_MatchStringStart(const char *str, const char *start);
com_internal void  COM_CopyStringA(const char *source, char *target);
com_internal void  COM_CopyString(const char *source, char *target);
com_internal i32   COM_CopyStringLimited(const char *source, char *target, i32 limit);
com_internal void  COM_CopyStringCount(const char *source, char *target, i32 count);
com_internal i32   COM_StrLenA(const char* str);
com_internal i32   COM_StrLen(const char* str);
com_internal i32   COM_StrReplace(char* str, char target, char replacement);
com_internal void  COM_StrToUpperCase(char* str);
com_internal void  COM_StrToLowerCase(char* str);
com_internal u8    COM_CharIsDecimalInt32(const char c);
com_internal u8    COM_AsciIsDecimalInt32(const char *str);
com_internal i32   COM_AsciToInt32(const char *str);
com_internal f32   COM_AsciToFloat32(const char *str);
com_internal i32   COM_StripTrailingInteger(char* text, char separator, i32 failureValue);
com_internal u8    COM_CheckForFileExtension(const char* filePath, const char* extension);
com_internal i32   COM_ConcatonateTokens(
	                        char* buffer,
	                        i32 bufferSize,
	                        char** strings,
	                        i32 numStrings,
	                        i32 firstString);
