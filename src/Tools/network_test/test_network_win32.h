#pragma once

#pragma comment(lib, "ws2_32.lib") // winsock lib

#include <conio.h>
#include <stdio.h>

#include "../../common/com_module.h"

#include "../../platform/win32_net/win32_net_interface.h"
#include "../../network/znet_interface.h"

#if 1
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>

// Example from:
// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-recvfrom
// https://www.binarytides.com/udp-socket-programming-in-winsock/
#endif

//#include "../../App/app_network_stream.h"

/////////////////////////////////////////////////////////////////
// Entry point
/////////////////////////////////////////////////////////////////
#define TEST_SERVER_PORT 23232
#define TEST_CLIENT_PORT 61200

void Test_NetworkWin32(i32 argc, char* argv[])
{
    COM_ASSERT(0, "Test");
}
