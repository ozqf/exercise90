#pragma once

#include "com_module.h"

i32 COM_ReadTokens(char* source, char* destination, char** tokens);
char* COM_RunToNewLine(char* buffer);
