#pragma once

COM_FatalErrorFunction com_fatalErrorFunc = NULL;

void COM_SetFatalError(COM_FatalErrorFunction func)
{
    printf("COM Set error handler\n");
    com_fatalErrorFunc = func;
}

void COM_Fatal(char* msg)
{
	if (com_fatalErrorFunc == NULL) { ILLEGAL_CODE_PATH; }
	com_fatalErrorFunc(msg, "Fatal error");
}
